from os import path
import re

from lib.pybars import Compiler
from lib.utils.DevTreeUtils import DevTree
from lib.utils.Logger import Log
from lib.utils.ProcessUtils import ExecutableRunner

UINT32 = "uint32"

HEADER_TEMPLATE = "{{#each commands}}{{>command}}{{>br}}{{/each}}"

COMMAND_STRUCT_TEMPLATE = """
struct {{name}}Command : public RenderHardwareCommand {
  public:
    {{name}}Command(
        {{#each attributes}}
            {{#if ref_param}}const{{/if}}
            {{{type}}}
            {{#if ref_param}}&{{/if}}
            {{name}}


            {{#unless @last}}, {{/unless}}
        {{/each}})

        {{#unless attributes}} = default;{{/unless}}

        {{#each attributes}}
            {{#if @first}}:{{/if}}

            {{name}}({{name}})

            {{#if @last}}
                {}
            {{else}}
                ,
            {{/if}}
        {{/each}}
    virtual ~{{name}}Command() = default;{{>br}}{{>br}}

    virtual void Execute(RenderHardwareInterface& rhi) const {
        rhi.{{name}}(
            {{#each attributes}}
                {{name}}

                {{#unless @last}},{{/unless}}
            {{/each}}
            );
    }{{>br}}{{>br}}

    virtual void Visualize(std::ostream& ostream) const {
        {{{visualizer}}}
    }{{>br}}{{>br}}

    {{#each attributes}}
        const {{{type}}} {{name}};
    {{/each}}
};{{>br}}
"""

COMMAND_METHOD_TEMPLATE = """
    virtual void {{name}}(
        {{#each attributes}}
            {{#if ref_param}}const{{/if}}
            {{{type}}}
            {{#if ref_param}}&{{/if}}
            {{name}}
            {{#unless @last}},{{/unless}}
        {{/each}}
    ) = 0;
"""


class RHICommand:
    def __init__(self, command_name, attributes=list(), visualizer=None):
        self.name = command_name
        self.attributes = attributes
        self.visualizer = visualizer if visualizer else 'ostream << "{0}";'.format(command_name)

    def to_dict(self):
        return {"name": self.name, "attributes": [attrib.to_dict() for attrib in self.attributes], 'visualizer': self.visualizer}


class RHICmdAttribute:
    def __init__(self, name, type, ref_param=True):
        self.name = name
        self.type = type
        self.ref_param = ref_param

    def to_dict(self):
        return {"name": self.name, "type": self.type, "ref_param": self.ref_param}


rhi_commands = [
    RHICommand("SetViewport", [
        RHICmdAttribute("x", "int32", False),
        RHICmdAttribute("y", "int32", False),
        RHICmdAttribute("width", "int32", False),
        RHICmdAttribute("height", "int32", False)
    ], """
    ostream << "SetViewport {";
    ostream << "x: " << x;
    ostream << " y: " << y;
    ostream << " width: " << width;
    ostream << " height: " << height;
    ostream << "}";
    """),

    RHICommand("SetScissorRect", [
        RHICmdAttribute("x", "int32", False),
        RHICmdAttribute("y", "int32", False),
        RHICmdAttribute("width", "int32", False),
        RHICmdAttribute("height", "int32", False)
    ]),

    RHICommand("SetRenderTargets", [
        RHICmdAttribute("colorTargets", "std::vector<RenderTarget>"),
        RHICmdAttribute("depthStencilTarget", "RenderTarget")
    ]),

    RHICommand("EnableVertexBinding", [
        RHICmdAttribute("vertexBinding", "std::shared_ptr<VertexBinding>")
    ]),

    RHICommand("EnableShaderProgram", [
        RHICmdAttribute("shaderProgram", "std::shared_ptr<ShaderProgram>")
    ]),

    RHICommand("BindTexture2D", [
        RHICmdAttribute("texture", "std::shared_ptr<Texture2D>"),
        RHICmdAttribute("textureSlot", "uint32")
    ]),

    RHICommand("SetUniformParameter", [
        RHICmdAttribute("uniformParameter", "std::shared_ptr<UniformParameter::UniformIndex>"),
        RHICmdAttribute("value", "glm::mat4"),
    ]),

    RHICommand("SetIntUniformParameter", [
        RHICmdAttribute("uniformParameter", "std::shared_ptr<UniformParameter::UniformIndex>"),
        RHICmdAttribute("value", "uint32"),
    ]),

    RHICommand("DrawIndexedPrimitive", [
        RHICmdAttribute("indexBuffer", "std::shared_ptr<IndexBuffer>"),
        RHICmdAttribute("firstIndex", "uint32"),
        RHICmdAttribute("lastIndex", "uint32"),

    ]),

    RHICommand("ClearFramebuffer", [
       RHICmdAttribute("clearColor", "bool"),
       RHICmdAttribute("clearDepth", "bool"),
       RHICmdAttribute("clearStencil", "bool")
    ]),

    RHICommand("BeginFrame", [
        RHICmdAttribute("viewportClient", "std::shared_ptr<ViewportClient>")
    ]),
    RHICommand("EndFrame", [
        RHICmdAttribute("viewportClient", "std::shared_ptr<ViewportClient>"),
        RHICmdAttribute("presentSource", "RenderTarget")
    ]),
]


def generate_command_structs():
    working_dir = DevTree.source_dir
    header_file = path.join(working_dir, "glove", "rendering", "RenderHardwareCommands.hpp")

    compiler = Compiler()

    header_template = compiler.compile(HEADER_TEMPLATE)
    command_struct_template = compiler.compile(COMMAND_STRUCT_TEMPLATE)
    br_template = compiler.compile("##br##")

    template_output = header_template({'commands': [cmd.to_dict() for cmd in rhi_commands]},
                                      partials={"command": command_struct_template, "br": br_template})

    generate_regex = re.compile(r"(// \[GENERATE:COMMANDS\]\n)(.*)(// \[\\GENERATE:COMMANDS\])", re.M | re.S)

    output = "\g<1>"
    output += str(template_output).replace('\n', '').replace('##br##', '\n')
    output += "\g<3>"

    new_header_content = ""
    with open(header_file, 'r') as file:
        new_header_content = generate_regex.sub(output, file.read())
    with open(header_file, 'w+') as file:
        file.write(new_header_content)

    clang_format_runner = ExecutableRunner('clang-format', ['-i', header_file], working_dir,
                                           lambda: (Log.info("Formatted RHI commands header")),
                                           lambda: (Log.error("Failed to format RHI commands header")))
    clang_format_runner.run()

def generate_command_methods():
    working_dir = DevTree.source_dir
    header_file = path.join(working_dir, "glove", "rendering", "RenderHardwareInterface.hpp")

    compiler = Compiler()

    header_template = compiler.compile(HEADER_TEMPLATE)
    command_method_template = compiler.compile(COMMAND_METHOD_TEMPLATE)
    br_template = compiler.compile("##br##")

    template_output = header_template({'commands': [cmd.to_dict() for cmd in rhi_commands]},
                                      partials={"command": command_method_template, "br": br_template})

    generate_regex = re.compile(r"(// \[GENERATE:COMMAND_METHODS\]\n)(.*)(// \[\\GENERATE:COMMAND_METHODS\])", re.M | re.S)

    output = "\g<1>"
    output += str(template_output).replace('\n', '').replace('##br##', '\n')
    output += "\g<3>"

    new_header_content = ""
    with open(header_file, 'r') as file:
        new_header_content = generate_regex.sub(output, file.read())
    with open(header_file, 'w+') as file:
        file.write(new_header_content)

    clang_format_runner = ExecutableRunner('clang-format', ['-i', header_file], working_dir,
                                           lambda: (Log.info("Formatted RHI methods header")),
                                           lambda: (Log.error("Failed to format RHI commands header")))
    clang_format_runner.run()


def execute_command(args):
    Log.info("Generating RHI commands...")

    generate_command_structs()
    generate_command_methods()

    Log.info("Generated {0} RHI commands".format(len(rhi_commands)))
