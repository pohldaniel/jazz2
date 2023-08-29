#pragma once

#include "../../Base/StaticHashMap.h"

#include "GLUniformBlockCache.h"
#include "../RenderBuffersManager.h"

namespace nCine
{
	class GLShaderProgram;
	class GLBufferObject;

	/// A class to handle all the uniform blocks of a shader program using a hashmap
	class GLShaderUniformBlocks
	{
	public:
		static constexpr int UniformBlockCachesHashSize = 4;
		using UniformHashMapType = StaticHashMap<String, GLUniformBlockCache, UniformBlockCachesHashSize>;

		GLShaderUniformBlocks();
		explicit GLShaderUniformBlocks(GLShaderProgram* shaderProgram);
		GLShaderUniformBlocks(GLShaderProgram* shaderProgram, const char* includeOnly, const char* exclude);
		inline void setProgram(GLShaderProgram* shaderProgram) {
			setProgram(shaderProgram, nullptr, nullptr);
		}
		void setProgram(GLShaderProgram* shaderProgram, const char* includeOnly, const char* exclude);
		void setUniformsDataPointer(GLubyte* dataPointer);

		inline unsigned int numUniformBlocks() const {
			return uniformBlockCaches_.size();
		}
		inline bool hasUniformBlock(const char* name) const {
			return (uniformBlockCaches_.find(String::nullTerminatedView(name)) != nullptr);
		}
		GLUniformBlockCache* uniformBlock(const char* name);
		inline const UniformHashMapType allUniformBlocks() const {
			return uniformBlockCaches_;
		}
		void commitUniformBlocks();

		void bind();

	private:
		GLShaderProgram* shaderProgram_;
		/// Pointer to the data of the first uniform block
		GLubyte* dataPointer_;

		/// Uniform buffer parameters for binding
		RenderBuffersManager::Parameters uboParams_;

		UniformHashMapType uniformBlockCaches_;

		/// Imports the uniform blocks with the option of including only some or excluding others
		void importUniformBlocks(const char* includeOnly, const char* exclude);
	};
}
