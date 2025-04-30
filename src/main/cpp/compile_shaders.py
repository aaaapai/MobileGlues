import itertools
import os

# 定义所有可能的特性组合
vs_attributes = {
    # in
    'Position': ["vec3", "vec4"],
    'Color': [None, "vec4"],
    'Normal': [None, "vec3"],
    'UV0': [None, "vec2"],
    'UV1': [None, "vec2", "ivec2"],
    'UV2': [None, "vec2", "ivec2"],

    # out
    'vertexDistance': [None, "float"],
    'vertexColor': [None, "vec4"],
    'texCoord0': [None, "vec2"],
    'texCoord1': [None, "vec2"],
    'texCoord2': [None, "vec2"],

    # uniform
}

fs_attributes = {
    # in
    'vertexDistance': [True, False],
    'vertexColor': [True, False],
    'texCoord0': [True, False],
    'texCoord1': [True, False],
    'texCoord2': [True, False],

    # out

    # uniform

}

# 生成所有组合
for combo in itertools.product(*vs_attributes.values()):
    defines = []
    define_str = ""
    # for idx, (k, v) in enumerate(vs_attributes.items()):
    #     if combo[idx]:
    #         defines.append(f"#define {k} 1")
    #         if v is not None:
    #             define_str += f"-D{k}={v} "
    for (k,v) in zip(vs_attributes.keys(), combo):
        # print(f"{k} = {v}", end=", ")
        if v is not None:
            define_str += f"-DUSE_{k}={v} "

    print(define_str)
    # 生成带预处理器指令的Shader
    # shader_src = "\n".join(defines) + "\n" + open("base.vert").read()
    variant_name = f"{'__'.join([f'{k}_{0 if (v is None) else v}' for k,v in zip(vs_attributes.keys(), combo)])}"

    print(variant_name)
    compile_cmd = f"glslc shaders/shader.vert -o shaders/spv/shader_{variant_name}.spv --target-env=opengl -Ishaders/include {define_str}"
    print(compile_cmd)
    os.system(compile_cmd)
    print()

#glslc -o out/simple.spv src/simple.vert --target-env=opengl -Iinclude
