#pragma once

#define NCINE_INCLUDE_OPENGL
#include "../../CommonHeaders.h"

#include "GLUniform.h"
#include "../../Base/StaticHashMapIterator.h"

namespace nCine
{
	class GLShaderProgram;

	/// A class to store information about an OpenGL uniform block
	class GLUniformBlock
	{
	public:
		static constexpr int MaxNameLength = 48;

		enum class DiscoverUniforms
		{
			ENABLED,
			DISABLED
		};

		GLUniformBlock();
		GLUniformBlock(GLuint program, GLuint blockIndex, DiscoverUniforms discover);
		GLUniformBlock(GLuint program, GLuint blockIndex);

		inline GLuint index() const {
			return index_;
		}
		inline GLint bindingIndex() const {
			return bindingIndex_;
		}
		/// Returns the size of the block aligned to the uniform buffer offset
		inline GLint size() const {
			return size_;
		}
		/// Returns the uniform buffer offset alignment added to the original size
		inline unsigned char alignAmount() const {
			return alignAmount_;
		}
		inline const char* name() const {
			return name_;
		}

		inline GLUniform* uniform(const char* name) {
			return blockUniforms_.find(String::nullTerminatedView(name));
		}
		void setBlockBinding(GLuint blockBinding);

	private:
		/// Max number of discoverable uniforms per block
		static const int MaxNumBlockUniforms = 16;

		static const int BlockUniformHashSize = 8;
		StaticHashMap<String, GLUniform, BlockUniformHashSize> blockUniforms_;

		GLuint program_;
		GLuint index_;
		/// Offset aligned size for `glBindBufferRange()` calls
		GLint size_;
		/// Uniform buffer offset alignment added to `size_`
		unsigned char alignAmount_;
		/// Current binding index for the uniform block. Negative if not bound.
		GLint bindingIndex_;
		char name_[MaxNameLength];

		friend class GLUniformBlockCache;
	};

}
