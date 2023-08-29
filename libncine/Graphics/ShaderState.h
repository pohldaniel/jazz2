#pragma once

#include "../../Common.h"
#include "../Primitives/Vector4.h"
#include "../Primitives/Colorf.h"

namespace nCine
{
	class DrawableNode;
	class Shader;
	class Texture;

	/// Shader state class for the user to use custom shaders
	class ShaderState
	{
	public:
		ShaderState();
		ShaderState(DrawableNode* node, Shader* shader);
		~ShaderState();

		inline const DrawableNode* node() const {
			return node_;
		}
		bool setNode(DrawableNode* node);

		inline const Shader* shader() const {
			return shader_;
		}
		bool setShader(Shader* shader);
		/// Triggers a shader update without setting a new shader
		bool resetShader();

		bool setTexture(unsigned int unit, const Texture* texture);
		inline bool setTexture(const Texture* texture) {
			return setTexture(0, texture);
		}

		bool setUniformInt(const char* blockName, const char* name, const int* vector);
		bool setUniformInt(const char* blockName, const char* name, int value0);
		bool setUniformInt(const char* blockName, const char* name, int value0, int value1);
		bool setUniformInt(const char* blockName, const char* name, int value0, int value1, int value2);
		bool setUniformInt(const char* blockName, const char* name, int value0, int value1, int value2, int value3);

		bool setUniformInt(const char* blockName, const char* name, const Vector2i& vector);
		bool setUniformInt(const char* blockName, const char* name, const Vector3i& vector);
		bool setUniformInt(const char* blockName, const char* name, const Vector4i& vector);

		bool setUniformFloat(const char* blockName, const char* name, const float* vector);
		bool setUniformFloat(const char* blockName, const char* name, float value0);
		bool setUniformFloat(const char* blockName, const char* name, float value0, float value1);
		bool setUniformFloat(const char* blockName, const char* name, float value0, float value1, float value2);
		bool setUniformFloat(const char* blockName, const char* name, float value0, float value1, float value2, float value3);

		bool setUniformFloat(const char* blockName, const char* name, const Vector2f& vector);
		bool setUniformFloat(const char* blockName, const char* name, const Vector3f& vector);
		bool setUniformFloat(const char* blockName, const char* name, const Vector4f& vector);
		bool setUniformFloat(const char* blockName, const char* name, const Colorf& color);

		unsigned int uniformBlockSize(const char* blockName);
		bool copyToUniformBlock(const char* blockName, unsigned int destIndex, unsigned char* src, unsigned int numBytes);
		bool copyToUniformBlock(const char* blockName, unsigned char* src, unsigned int numBytes);
		bool copyToUniformBlock(const char* blockName, unsigned char* src);

	private:
		DrawableNode* node_;
		Shader* shader_;
		int previousShaderType_;

		/// Deleted copy constructor
		ShaderState(const ShaderState&) = delete;
		/// Deleted assignment operator
		ShaderState& operator=(const ShaderState&) = delete;
	};

}