#pragma once
#include "Mesh.h"

using namespace DirectX;

class Model
{
public:
	bool Initialize(
		const std::string& filePath,
		ID3D11Device* device,
		ID3D11DeviceContext* context,
		ConstantBuffer<CB_VS_matrix>& cb_vs_vertexshader );
	void Draw( const XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix );
private:
	bool LoadModel( const std::string& filePath );
	void ProcessNode( aiNode* node, const aiScene* scene, const XMMATRIX& parentTransformMatrix );
	Mesh ProcessMesh( aiMesh* mesh, const aiScene* scene, const XMMATRIX& transformMatrix );
	TextureStorageType GetTextureStorageType( const aiScene* pScene, aiMaterial* pMaterial, unsigned int index, aiTextureType textureType );
	std::vector<Texture> LoadMaterialTextures( aiMaterial* pMaterial, aiTextureType textureType, const aiScene* pScene );
	int GetTextureIndex( aiString* pStr );
private:
	std::string directory = "";
	std::vector<Mesh> meshes;
	ID3D11Device* device = nullptr;
	ID3D11DeviceContext* context = nullptr;
	ConstantBuffer<CB_VS_matrix>* cb_vs_vertexshader = nullptr;
};