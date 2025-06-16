#pragma once

# include <engine/engine.hpp>
# include <engine/mesh/Mesh.hpp>
# include <engine/mesh/VertexPosTex.hpp>
# include <engine/shader/Shader.hpp>
# include <engine/camera/Camera.hpp>
# include <program/map/Cube.hpp>
# include <program/shaderStruct.hpp>
# include <program/map/PerlinNoise.hpp>

class Skybox {
public:
    Skybox();
    ~Skybox();

    void init(Engine &engine, Shader &skyboxShader, Camera &camera);
    void destroy(Engine &engine);
    void draw(Engine &engine, Camera &camera, Shader &skyboxShader);

private:
    Mesh<VertexPosTex> mesh;
    ShaderParam		shaderParam;
    UBO3DChunkPos          uboPos;
};