//
// Created by Swung 0x48 on 2025/4/15.
//
#include "gtest/gtest.h"

#include "../../gl/fpe/types.h"
#include "../../gl/fpe/vertexpointer.h"

TEST(VertexPointer, PointerNormalize0x83) {
    struct test_struct_t {
        float pos[4];
        float normal[3];
        float uv[2];
    } ts;

    auto& raw_vpa = g_glstate.fpe_state.vertexpointer_array;

    raw_vpa.reset();

    glVertexPointer(4, GL_FLOAT, sizeof(test_struct_t), ts.pos);
    glNormalPointer(GL_FLOAT, sizeof(test_struct_t), ts.normal);
    glTexCoordPointer(2, GL_FLOAT, sizeof(test_struct_t), ts.uv);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    auto vpa = raw_vpa.normalize();

    EXPECT_EQ((uint64_t) &ts,
              (uint64_t) vpa.starting_pointer);

    EXPECT_EQ((uint64_t) 0x83,
              (uint64_t) vpa.enabled_pointers);

    EXPECT_EQ((uint64_t) sizeof(test_struct_t),
              (uint64_t) vpa.stride);

    auto& vertex_va = vpa.attributes[vp2idx(GL_VERTEX_ARRAY)];
    EXPECT_EQ(vertex_va.size, 4);
    EXPECT_EQ(vertex_va.usage, GL_VERTEX_ARRAY);
    EXPECT_EQ(vertex_va.type, GL_FLOAT);
    EXPECT_EQ(vertex_va.stride, sizeof(test_struct_t));
    EXPECT_EQ(vertex_va.pointer, (const void*)0);

    auto& normal_va = vpa.attributes[vp2idx(GL_NORMAL_ARRAY)];
    EXPECT_EQ(normal_va.size, 3);
    EXPECT_EQ(normal_va.usage, GL_NORMAL_ARRAY);
    EXPECT_EQ(normal_va.type, GL_FLOAT);
    EXPECT_EQ(normal_va.stride, sizeof(test_struct_t));
    EXPECT_EQ(normal_va.pointer, (const void*) offsetof(test_struct_t, normal));

    auto& tex_va = vpa.attributes[vp2idx(GL_TEXTURE_COORD_ARRAY)];
    EXPECT_EQ(tex_va.size, 2);
    EXPECT_EQ(tex_va.usage, GL_TEXTURE_COORD_ARRAY);
    EXPECT_EQ(tex_va.type, GL_FLOAT);
    EXPECT_EQ(tex_va.stride, sizeof(test_struct_t));
    EXPECT_EQ(tex_va.pointer, (const void*) offsetof(test_struct_t, uv));

    // Check for idempotence
    for (int i = 0; i < 5; ++i) {
        auto vpa = raw_vpa.normalize();

        EXPECT_EQ((uint64_t) &ts,
                  (uint64_t) vpa.starting_pointer);

        EXPECT_EQ((uint64_t) 0x83,
                  (uint64_t) vpa.enabled_pointers);

        EXPECT_EQ((uint64_t) sizeof(test_struct_t),
                  (uint64_t) vpa.stride);

        auto& vertex_va = vpa.attributes[vp2idx(GL_VERTEX_ARRAY)];
        EXPECT_EQ(vertex_va.size, 4);
        EXPECT_EQ(vertex_va.usage, GL_VERTEX_ARRAY);
        EXPECT_EQ(vertex_va.type, GL_FLOAT);
        EXPECT_EQ(vertex_va.stride, sizeof(test_struct_t));
        EXPECT_EQ(vertex_va.pointer, (const void*)0);

        auto& normal_va = vpa.attributes[vp2idx(GL_NORMAL_ARRAY)];
        EXPECT_EQ(normal_va.size, 3);
        EXPECT_EQ(normal_va.usage, GL_NORMAL_ARRAY);
        EXPECT_EQ(normal_va.type, GL_FLOAT);
        EXPECT_EQ(normal_va.stride, sizeof(test_struct_t));
        EXPECT_EQ(normal_va.pointer, (const void*) offsetof(test_struct_t, normal));

        auto& tex_va = vpa.attributes[vp2idx(GL_TEXTURE_COORD_ARRAY)];
        EXPECT_EQ(tex_va.size, 2);
        EXPECT_EQ(tex_va.usage, GL_TEXTURE_COORD_ARRAY);
        EXPECT_EQ(tex_va.type, GL_FLOAT);
        EXPECT_EQ(tex_va.stride, sizeof(test_struct_t));
        EXPECT_EQ(tex_va.pointer, (const void*) offsetof(test_struct_t, uv));
    }
}

TEST(VertexPointer, PointerNormalize0x5) {
    struct test_struct_t {
        float pos[4];
        uint8_t color[4];
    } ts;

    auto& raw_vpa = g_glstate.fpe_state.vertexpointer_array;

    raw_vpa.reset();

    glVertexPointer(4, GL_FLOAT, sizeof(test_struct_t), ts.pos);
    glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(test_struct_t), ts.color);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

    auto vpa = raw_vpa.normalize();

    EXPECT_EQ((uint64_t) &ts,
              (uint64_t) vpa.starting_pointer);

    EXPECT_EQ((uint64_t) 0x5,
              (uint64_t) vpa.enabled_pointers);

    EXPECT_EQ((uint64_t) sizeof(test_struct_t),
              (uint64_t) vpa.stride);

    auto& vertex_va = vpa.attributes[vp2idx(GL_VERTEX_ARRAY)];
    EXPECT_EQ(vertex_va.size, 4);
    EXPECT_EQ(vertex_va.usage, GL_VERTEX_ARRAY);
    EXPECT_EQ(vertex_va.type, GL_FLOAT);
    EXPECT_EQ(vertex_va.stride, sizeof(test_struct_t));
    EXPECT_EQ(vertex_va.pointer, (const void*)0);

    auto& color_va = vpa.attributes[vp2idx(GL_COLOR_ARRAY)];
    EXPECT_EQ(color_va.size, 4);
    EXPECT_EQ(color_va.usage, GL_COLOR_ARRAY);
    EXPECT_EQ(color_va.type, GL_UNSIGNED_BYTE);
    EXPECT_EQ(color_va.stride, sizeof(test_struct_t));
    EXPECT_EQ(color_va.pointer, (const void*) offsetof(test_struct_t, color));

    // Check for idempotence
    for (int i = 0; i < 5; ++i) {
        auto vpa = raw_vpa.normalize();

        EXPECT_EQ((uint64_t) &ts,
                  (uint64_t) vpa.starting_pointer);

        EXPECT_EQ((uint64_t) 0x5,
                  (uint64_t) vpa.enabled_pointers);

        EXPECT_EQ((uint64_t) sizeof(test_struct_t),
                  (uint64_t) vpa.stride);

        auto& vertex_va = vpa.attributes[vp2idx(GL_VERTEX_ARRAY)];
        EXPECT_EQ(vertex_va.size, 4);
        EXPECT_EQ(vertex_va.usage, GL_VERTEX_ARRAY);
        EXPECT_EQ(vertex_va.type, GL_FLOAT);
        EXPECT_EQ(vertex_va.stride, sizeof(test_struct_t));
        EXPECT_EQ(vertex_va.pointer, (const void*)0);

        auto& color_va = vpa.attributes[vp2idx(GL_COLOR_ARRAY)];
        EXPECT_EQ(color_va.size, 4);
        EXPECT_EQ(color_va.usage, GL_COLOR_ARRAY);
        EXPECT_EQ(color_va.type, GL_UNSIGNED_BYTE);
        EXPECT_EQ(color_va.stride, sizeof(test_struct_t));
        EXPECT_EQ(color_va.pointer, (const void*) offsetof(test_struct_t, color));
    }
}

TEST(VertexPointer, PointerNormalize0x85) {
    auto& raw_vpa = g_glstate.fpe_state.vertexpointer_array;

    raw_vpa.reset();

    glVertexPointer(3, GL_FLOAT, 24, (const void*)0xe9d6a000);
    glEnableClientState(GL_VERTEX_ARRAY);
    glTexCoordPointer(2, GL_FLOAT, 24, (const void*)0xe9d6a00c);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glColorPointer(4, GL_UNSIGNED_BYTE, 24, (const void*)0xe9d6a014);
    glEnableClientState(GL_COLOR_ARRAY);

    auto vpa = raw_vpa.normalize();

    EXPECT_EQ((uint64_t) 0xe9d6a000,
              (uint64_t) vpa.starting_pointer);

    EXPECT_EQ((uint64_t) 0x85,
              (uint64_t) vpa.enabled_pointers);

    EXPECT_EQ((uint64_t) 24,
              (uint64_t) vpa.stride);

    auto& vertex_va = vpa.attributes[vp2idx(GL_VERTEX_ARRAY)];
    EXPECT_EQ(vertex_va.size, 3);
    EXPECT_EQ(vertex_va.usage, GL_VERTEX_ARRAY);
    EXPECT_EQ(vertex_va.type, GL_FLOAT);
    EXPECT_EQ(vertex_va.stride, 24);
    EXPECT_EQ(vertex_va.pointer, (const void*)0);

    auto& color_va = vpa.attributes[vp2idx(GL_COLOR_ARRAY)];
    EXPECT_EQ(color_va.size, 4);
    EXPECT_EQ(color_va.usage, GL_COLOR_ARRAY);
    EXPECT_EQ(color_va.type, GL_UNSIGNED_BYTE);
    EXPECT_EQ(color_va.stride, 24);
    EXPECT_EQ(color_va.pointer, (const void*) 0x14);

    auto& tex_va = vpa.attributes[vp2idx(GL_TEXTURE_COORD_ARRAY)];
    EXPECT_EQ(tex_va.size, 2);
    EXPECT_EQ(tex_va.usage, GL_TEXTURE_COORD_ARRAY);
    EXPECT_EQ(tex_va.type, GL_FLOAT);
    EXPECT_EQ(tex_va.stride, 24);
    EXPECT_EQ(tex_va.pointer, (const void*) 0xc);

    // Check for idempotence
    for (int i = 0; i < 5; ++i) {
        auto vpa = raw_vpa.normalize();

        EXPECT_EQ((uint64_t) 0xe9d6a000,
                  (uint64_t) vpa.starting_pointer);

        EXPECT_EQ((uint64_t) 0x85,
                  (uint64_t) vpa.enabled_pointers);

        EXPECT_EQ((uint64_t) 24,
                  (uint64_t) vpa.stride);

        auto& vertex_va = vpa.attributes[vp2idx(GL_VERTEX_ARRAY)];
        EXPECT_EQ(vertex_va.size, 3);
        EXPECT_EQ(vertex_va.usage, GL_VERTEX_ARRAY);
        EXPECT_EQ(vertex_va.type, GL_FLOAT);
        EXPECT_EQ(vertex_va.stride, 24);
        EXPECT_EQ(vertex_va.pointer, (const void*)0);

        auto& color_va = vpa.attributes[vp2idx(GL_COLOR_ARRAY)];
        EXPECT_EQ(color_va.size, 4);
        EXPECT_EQ(color_va.usage, GL_COLOR_ARRAY);
        EXPECT_EQ(color_va.type, GL_UNSIGNED_BYTE);
        EXPECT_EQ(color_va.stride, 24);
        EXPECT_EQ(color_va.pointer, (const void*) 0x14);

        auto& tex_va = vpa.attributes[vp2idx(GL_TEXTURE_COORD_ARRAY)];
        EXPECT_EQ(tex_va.size, 2);
        EXPECT_EQ(tex_va.usage, GL_TEXTURE_COORD_ARRAY);
        EXPECT_EQ(tex_va.type, GL_FLOAT);
        EXPECT_EQ(tex_va.stride, 24);
        EXPECT_EQ(tex_va.pointer, (const void*) 0xc);
    }
}

TEST(VertexPointer, PointerNormalizeMix) {
    auto& raw_vpa = g_glstate.fpe_state.vertexpointer_array;

    raw_vpa.reset();

    glVertexPointer(3, GL_FLOAT, 24, (const void*)0xe9d6a000);
    glEnableClientState(GL_VERTEX_ARRAY);
    glTexCoordPointer(2, GL_FLOAT, 24, (const void*)0xe9d6a00c);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glColorPointer(4, GL_UNSIGNED_BYTE, 24, (const void*)0xe9d6a014);
    glEnableClientState(GL_COLOR_ARRAY);

    auto vpa = raw_vpa.normalize();

    EXPECT_EQ((uint64_t) 0xe9d6a000,
              (uint64_t) vpa.starting_pointer);

    EXPECT_EQ((uint64_t) 0x85,
              (uint64_t) vpa.enabled_pointers);

    EXPECT_EQ((uint64_t) 24,
              (uint64_t) vpa.stride);

    {
        auto &vertex_va = vpa.attributes[vp2idx(GL_VERTEX_ARRAY)];
        EXPECT_EQ(vertex_va.size, 3);
        EXPECT_EQ(vertex_va.usage, GL_VERTEX_ARRAY);
        EXPECT_EQ(vertex_va.type, GL_FLOAT);
        EXPECT_EQ(vertex_va.stride, 24);
        EXPECT_EQ(vertex_va.pointer, (const void *) 0);

        auto &color_va = vpa.attributes[vp2idx(GL_COLOR_ARRAY)];
        EXPECT_EQ(color_va.size, 4);
        EXPECT_EQ(color_va.usage, GL_COLOR_ARRAY);
        EXPECT_EQ(color_va.type, GL_UNSIGNED_BYTE);
        EXPECT_EQ(color_va.stride, 24);
        EXPECT_EQ(color_va.pointer, (const void *) 0x14);

        auto &tex_va = vpa.attributes[vp2idx(GL_TEXTURE_COORD_ARRAY)];
        EXPECT_EQ(tex_va.size, 2);
        EXPECT_EQ(tex_va.usage, GL_TEXTURE_COORD_ARRAY);
        EXPECT_EQ(tex_va.type, GL_FLOAT);
        EXPECT_EQ(tex_va.stride, 24);
        EXPECT_EQ(tex_va.pointer, (const void *) 0xc);
    }
    // Check for idempotence
    for (int i = 0; i < 5; ++i) {
        auto vpa = raw_vpa.normalize();

        EXPECT_EQ((uint64_t) 0xe9d6a000,
                  (uint64_t) vpa.starting_pointer);

        EXPECT_EQ((uint64_t) 0x85,
                  (uint64_t) vpa.enabled_pointers);

        EXPECT_EQ((uint64_t) 24,
                  (uint64_t) vpa.stride);

        auto& vertex_va = vpa.attributes[vp2idx(GL_VERTEX_ARRAY)];
        EXPECT_EQ(vertex_va.size, 3);
        EXPECT_EQ(vertex_va.usage, GL_VERTEX_ARRAY);
        EXPECT_EQ(vertex_va.type, GL_FLOAT);
        EXPECT_EQ(vertex_va.stride, 24);
        EXPECT_EQ(vertex_va.pointer, (const void*)0);

        auto& color_va = vpa.attributes[vp2idx(GL_COLOR_ARRAY)];
        EXPECT_EQ(color_va.size, 4);
        EXPECT_EQ(color_va.usage, GL_COLOR_ARRAY);
        EXPECT_EQ(color_va.type, GL_UNSIGNED_BYTE);
        EXPECT_EQ(color_va.stride, 24);
        EXPECT_EQ(color_va.pointer, (const void*) 0x14);

        auto& tex_va = vpa.attributes[vp2idx(GL_TEXTURE_COORD_ARRAY)];
        EXPECT_EQ(tex_va.size, 2);
        EXPECT_EQ(tex_va.usage, GL_TEXTURE_COORD_ARRAY);
        EXPECT_EQ(tex_va.type, GL_FLOAT);
        EXPECT_EQ(tex_va.stride, 24);
        EXPECT_EQ(tex_va.pointer, (const void*) 0xc);
    }

    // OpenGL code should take care of this using glEnable/DisableClientState
    raw_vpa.enabled_pointers = 0;

    struct test_struct_t {
        float pos[4];
        float normal[3];
        float uv[2];
    } ts;

    glVertexPointer(4, GL_FLOAT, sizeof(test_struct_t), ts.pos);
    glNormalPointer(GL_FLOAT, sizeof(test_struct_t), ts.normal);
    glTexCoordPointer(2, GL_FLOAT, sizeof(test_struct_t), ts.uv);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    // Let's see how does assign (not copy-construct) work
    vpa = raw_vpa.normalize();

    EXPECT_EQ((uint64_t) &ts,
              (uint64_t) vpa.starting_pointer);

    EXPECT_EQ((uint64_t) 0x83,
              (uint64_t) vpa.enabled_pointers);

    EXPECT_EQ((uint64_t) sizeof(test_struct_t),
              (uint64_t) vpa.stride);

    {
        auto &vertex_va = vpa.attributes[vp2idx(GL_VERTEX_ARRAY)];
        EXPECT_EQ(vertex_va.size, 4);
        EXPECT_EQ(vertex_va.usage, GL_VERTEX_ARRAY);
        EXPECT_EQ(vertex_va.type, GL_FLOAT);
        EXPECT_EQ(vertex_va.stride, sizeof(test_struct_t));
        EXPECT_EQ(vertex_va.pointer, (const void *) 0);

        auto &normal_va = vpa.attributes[vp2idx(GL_NORMAL_ARRAY)];
        EXPECT_EQ(normal_va.size, 3);
        EXPECT_EQ(normal_va.usage, GL_NORMAL_ARRAY);
        EXPECT_EQ(normal_va.type, GL_FLOAT);
        EXPECT_EQ(normal_va.stride, sizeof(test_struct_t));
        EXPECT_EQ(normal_va.pointer, (const void *) offsetof(test_struct_t, normal));

        auto &tex_va = vpa.attributes[vp2idx(GL_TEXTURE_COORD_ARRAY)];
        EXPECT_EQ(tex_va.size, 2);
        EXPECT_EQ(tex_va.usage, GL_TEXTURE_COORD_ARRAY);
        EXPECT_EQ(tex_va.type, GL_FLOAT);
        EXPECT_EQ(tex_va.stride, sizeof(test_struct_t));
        EXPECT_EQ(tex_va.pointer, (const void *) offsetof(test_struct_t, uv));
    }

    // Check for idempotence
    for (int i = 0; i < 5; ++i) {
        vpa = raw_vpa.normalize();

        EXPECT_EQ((uint64_t) &ts,
                  (uint64_t) vpa.starting_pointer);

        EXPECT_EQ((uint64_t) 0x83,
                  (uint64_t) vpa.enabled_pointers);

        EXPECT_EQ((uint64_t) sizeof(test_struct_t),
                  (uint64_t) vpa.stride);

        auto& vertex_va = vpa.attributes[vp2idx(GL_VERTEX_ARRAY)];
        EXPECT_EQ(vertex_va.size, 4);
        EXPECT_EQ(vertex_va.usage, GL_VERTEX_ARRAY);
        EXPECT_EQ(vertex_va.type, GL_FLOAT);
        EXPECT_EQ(vertex_va.stride, sizeof(test_struct_t));
        EXPECT_EQ(vertex_va.pointer, (const void*)0);

        auto& normal_va = vpa.attributes[vp2idx(GL_NORMAL_ARRAY)];
        EXPECT_EQ(normal_va.size, 3);
        EXPECT_EQ(normal_va.usage, GL_NORMAL_ARRAY);
        EXPECT_EQ(normal_va.type, GL_FLOAT);
        EXPECT_EQ(normal_va.stride, sizeof(test_struct_t));
        EXPECT_EQ(normal_va.pointer, (const void*) offsetof(test_struct_t, normal));

        auto& tex_va = vpa.attributes[vp2idx(GL_TEXTURE_COORD_ARRAY)];
        EXPECT_EQ(tex_va.size, 2);
        EXPECT_EQ(tex_va.usage, GL_TEXTURE_COORD_ARRAY);
        EXPECT_EQ(tex_va.type, GL_FLOAT);
        EXPECT_EQ(tex_va.stride, sizeof(test_struct_t));
        EXPECT_EQ(tex_va.pointer, (const void*) offsetof(test_struct_t, uv));
    }
}

TEST(VertexPointer, PointerNormalizeZeroStride) {
    struct test_struct_t {
        float pos[4];
        float normal[3];
        float uv[2];
    };

    auto& raw_vpa = g_glstate.fpe_state.vertexpointer_array;

    raw_vpa.reset();

    glVertexPointer(4, GL_FLOAT, 0, (const void*)offsetof(test_struct_t, pos));
    glNormalPointer(GL_FLOAT, 0, (const void*)offsetof(test_struct_t, normal));
    glTexCoordPointer(2, GL_FLOAT, 0, (const void*)offsetof(test_struct_t, uv));

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    auto vpa = raw_vpa.normalize();

    EXPECT_EQ((uint64_t) 0,
              (uint64_t) vpa.starting_pointer);

    EXPECT_EQ((uint64_t) 0x83,
              (uint64_t) vpa.enabled_pointers);

    EXPECT_EQ((uint64_t) sizeof(test_struct_t),
              (uint64_t) vpa.stride);

    auto& vertex_va = vpa.attributes[vp2idx(GL_VERTEX_ARRAY)];
    EXPECT_EQ(vertex_va.size, 4);
    EXPECT_EQ(vertex_va.usage, GL_VERTEX_ARRAY);
    EXPECT_EQ(vertex_va.type, GL_FLOAT);
    EXPECT_EQ(vertex_va.stride, sizeof(test_struct_t));
    EXPECT_EQ(vertex_va.pointer, (const void*)0);

    auto& normal_va = vpa.attributes[vp2idx(GL_NORMAL_ARRAY)];
    EXPECT_EQ(normal_va.size, 3);
    EXPECT_EQ(normal_va.usage, GL_NORMAL_ARRAY);
    EXPECT_EQ(normal_va.type, GL_FLOAT);
    EXPECT_EQ(normal_va.stride, sizeof(test_struct_t));
    EXPECT_EQ(normal_va.pointer, (const void*) offsetof(test_struct_t, normal));

    auto& tex_va = vpa.attributes[vp2idx(GL_TEXTURE_COORD_ARRAY)];
    EXPECT_EQ(tex_va.size, 2);
    EXPECT_EQ(tex_va.usage, GL_TEXTURE_COORD_ARRAY);
    EXPECT_EQ(tex_va.type, GL_FLOAT);
    EXPECT_EQ(tex_va.stride, sizeof(test_struct_t));
    EXPECT_EQ(tex_va.pointer, (const void*) offsetof(test_struct_t, uv));

    // Check for idempotence
    for (int i = 0; i < 5; ++i) {
        auto vpa = raw_vpa.normalize();

        EXPECT_EQ((uint64_t) 0,
                  (uint64_t) vpa.starting_pointer);

        EXPECT_EQ((uint64_t) 0x83,
                  (uint64_t) vpa.enabled_pointers);

        EXPECT_EQ((uint64_t) sizeof(test_struct_t),
                  (uint64_t) vpa.stride);

        auto& vertex_va = vpa.attributes[vp2idx(GL_VERTEX_ARRAY)];
        EXPECT_EQ(vertex_va.size, 4);
        EXPECT_EQ(vertex_va.usage, GL_VERTEX_ARRAY);
        EXPECT_EQ(vertex_va.type, GL_FLOAT);
        EXPECT_EQ(vertex_va.stride, sizeof(test_struct_t));
        EXPECT_EQ(vertex_va.pointer, (const void*)0);

        auto& normal_va = vpa.attributes[vp2idx(GL_NORMAL_ARRAY)];
        EXPECT_EQ(normal_va.size, 3);
        EXPECT_EQ(normal_va.usage, GL_NORMAL_ARRAY);
        EXPECT_EQ(normal_va.type, GL_FLOAT);
        EXPECT_EQ(normal_va.stride, sizeof(test_struct_t));
        EXPECT_EQ(normal_va.pointer, (const void*) offsetof(test_struct_t, normal));

        auto& tex_va = vpa.attributes[vp2idx(GL_TEXTURE_COORD_ARRAY)];
        EXPECT_EQ(tex_va.size, 2);
        EXPECT_EQ(tex_va.usage, GL_TEXTURE_COORD_ARRAY);
        EXPECT_EQ(tex_va.type, GL_FLOAT);
        EXPECT_EQ(tex_va.stride, sizeof(test_struct_t));
        EXPECT_EQ(tex_va.pointer, (const void*) offsetof(test_struct_t, uv));
    }
}
