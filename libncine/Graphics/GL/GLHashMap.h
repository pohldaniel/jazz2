#pragma once

#include "../../../Common.h"

#define NCINE_INCLUDE_OPENGL
#include "../../CommonHeaders.h"

namespace nCine
{
	using key_t = GLenum;
	using value_t = GLuint;

	/// Naive implementation of a hashmap for storing pairs of OpenGL targets and object ids
	template <unsigned int S, class MappingFunc>
	class GLHashMap
	{
	public:
		GLHashMap();
		/// Returns the bucket index using the mapping function on the key
		value_t& operator[](key_t key);

	private:
		value_t buckets_[S];
		MappingFunc mappingFunc;
	};

	template <unsigned int S, class MappingFunc>
	GLHashMap<S, MappingFunc>::GLHashMap()
	{
		// Initializing with a null OpenGL object id
		for (unsigned int i = 0; i < S; i++)
			buckets_[i] = 0;
	}

	template <unsigned int S, class MappingFunc>
	inline value_t& GLHashMap<S, MappingFunc>::operator[](key_t key)
	{
		return buckets_[mappingFunc(key)];
	}

	/// A class to perform a mapping between OpenGL buffer object targets and array indices
	class GLBufferObjectMappingFunc
	{
	public:
		static const unsigned int Size = 6;
		inline unsigned int operator()(key_t key) const
		{
			unsigned int value = 0;

			switch (key) {
				case GL_ARRAY_BUFFER:
					value = 0;
					break;
				case GL_ELEMENT_ARRAY_BUFFER:
					value = 1;
					break;
				case GL_UNIFORM_BUFFER:
					value = 2;
					break;
				case GL_PIXEL_PACK_BUFFER:
					value = 3;
					break;
				case GL_PIXEL_UNPACK_BUFFER:
					value = 4;
					break;
#if !defined(WITH_OPENGLES) || (defined(WITH_OPENGLES) && GL_ES_VERSION_3_2)
				case GL_TEXTURE_BUFFER:
					value = 5;
					break;
#endif
				default:
					FATAL_MSG("No available case to handle buffer object target: 0x%x", key);
					break;
			}

			return value;
		}
	};

	/// A class to perform a mapping between OpenGL texture targets and array indices
	class GLTextureMappingFunc
	{
	public:
		static const unsigned int Size = 4;
		inline unsigned int operator()(key_t key) const
		{
			unsigned int value = 0;

			switch (key) {
#if !defined(WITH_OPENGLES) // not available in OpenGL ES
				case GL_TEXTURE_1D:
					value = 0;
					break;
#endif
				case GL_TEXTURE_2D:
					value = 1;
					break;
				case GL_TEXTURE_3D:
					value = 2;
					break;
#if !defined(WITH_OPENGLES) || (defined(WITH_OPENGLES) && GL_ES_VERSION_3_2)
				case GL_TEXTURE_BUFFER:
					value = 3;
					break;
#endif
				default:
					FATAL_MSG("No available case to handle texture target: 0x%x", key);
					break;
			}

			return value;
		}
	};

}
