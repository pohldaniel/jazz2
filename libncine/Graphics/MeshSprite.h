#pragma once

#include "BaseSprite.h"

#include <Containers/SmallVector.h>

using namespace Death::Containers;

namespace nCine
{
	/// A scene node representing a mesh with vertices and UVs
	class MeshSprite : public BaseSprite
	{
	public:
		/// Vertex data for the mesh
		struct Vertex
		{
			float x, y;
			float u, v;

			Vertex()
				: x(0.0f), y(0.0f), u(0.0f), v(0.0f) {}
			Vertex(float xx, float yy, float uu, float vv)
				: x(xx), y(yy), u(uu), v(vv) {}
		};
		static const unsigned int VertexBytes = sizeof(Vertex);
		static const unsigned int VertexFloats = VertexBytes / sizeof(float);

		/// Vertex data for the mesh when no texture is specified
		struct VertexNoTexture
		{
			float x, y;

			VertexNoTexture()
				: x(0.0f), y(0.0f) {}
			VertexNoTexture(float xx, float yy)
				: x(xx), y(yy) {}
		};
		static const unsigned int VertexNoTextureBytes = sizeof(VertexNoTexture);
		static const unsigned int VertexNoTextureFloats = VertexNoTextureBytes / sizeof(float);

		enum class TextureCutMode
		{
			RESIZE,
			CROP
		};

		/// Default constructor for a sprite with no parent and no texture, positioned in the origin
		MeshSprite();
		/// Constructor for a sprite with a parent and texture, positioned in the relative origin
		MeshSprite(SceneNode* parent, Texture* texture);
		/// Constructor for a sprite with a texture but no parent, positioned in the origin
		explicit MeshSprite(Texture* texture);
		/// Constructor for a sprite with a parent, a texture and a specified relative position
		MeshSprite(SceneNode* parent, Texture* texture, float xx, float yy);
		/// Constructor for a sprite with a parent, a texture and a specified relative position as a vector
		MeshSprite(SceneNode* parent, Texture* texture, const Vector2f& position);
		/// Constructor for a sprite with a texture and a specified position but no parent
		MeshSprite(Texture* texture, float xx, float yy);
		/// Constructor for a sprite with a texture and a specified position as a vector but no parent
		MeshSprite(Texture* texture, const Vector2f& position);

		/// Default move constructor
		MeshSprite(MeshSprite&&) = default;
		/// Default move assignment operator
		MeshSprite& operator=(MeshSprite&&) = default;

		/// Returns a copy of this object
		inline MeshSprite clone() const {
			return MeshSprite(*this);
		}

		/// Returns the number of bytes used by each vertex
		inline unsigned int bytesPerVertex() const {
			return bytesPerVertex_;
		}
		/// Returns the number of vertices of the sprite mesh
		inline unsigned int numVertices() const {
			return numVertices_;
		}
		/// Returns the total number of bytes used by all sprite's vertices
		inline unsigned int numBytes() const {
			return numVertices_ * bytesPerVertex_;
		}
		/// Returns the vertices data of the sprite mesh
		inline const float* vertices() const {
			return vertexDataPointer_;
		}
		/// Returns true if the vertices belong to the sprite and are not stored externally
		inline bool uniqueVertices() const {
			return vertexDataPointer_ == vertices_.data();
		}

		/// Copies the vertices data with a custom format from a pointer into the sprite
		void copyVertices(unsigned int numVertices, unsigned int bytesPerVertex, const void* vertexData);
		/// Copies the vertices data from a pointer into the sprite
		void copyVertices(unsigned int numVertices, const Vertex* vertices);
		/// Copies the vertices data from a pointer into the sprite (no texture version)
		void copyVertices(unsigned int numVertices, const VertexNoTexture* vertices);
		/// Copies the vertices data from another sprite and sets the same size
		void copyVertices(const MeshSprite& meshSprite);

		/// Sets the vertices data to point to an external array with a custom format
		void setVertices(unsigned int numVertices, unsigned int bytesPerVertex, const void* vertexData);
		/// Sets the vertices data to point to an external array
		void setVertices(unsigned int numVertices, const Vertex* vertices);
		/// Sets the vertices data to point to an external array (no texture version)
		void setVertices(unsigned int numVertices, const VertexNoTexture* vertices);
		/// Sets the vertices data to the data used by another sprite and sets the same size
		void setVertices(const MeshSprite& meshSprite);

		/// Returns the internal vertices data, cleared and set to the required size (custom format version)
		float* emplaceVertices(unsigned int numElements, unsigned int bytesPerVertex);
		/// Returns the internal vertices data, cleared and set to the required size
		float* emplaceVertices(unsigned int numElements);

		/// Creates an internal set of vertices from an external array of points in texture space, with optional texture cut mode
		void createVerticesFromTexels(unsigned int numVertices, const Vector2f* points, TextureCutMode cutMode);
		/// Creates an internal set of vertices from an external array of points in texture space
		void createVerticesFromTexels(unsigned int numVertices, const Vector2f* points);

		/// Returns the number of indices used to draw the sprite mesh
		inline unsigned int numIndices() const {
			return numIndices_;
		}
		/// Returns the indices used to draw the sprite mesh
		inline const unsigned short* indices() const {
			return indexDataPointer_;
		}
		/// Returns true if the indices belong to the sprite and are not stored externally
		inline bool uniqueIndices() const {
			return indexDataPointer_ == indices_.data();
		}
		/// Copies the indices from a pointer into the sprite
		void copyIndices(unsigned int numIndices, const unsigned short* indices);
		/// Copies the indices from another sprite
		void copyIndices(const MeshSprite& meshSprite);
		/// Sets the indices data to point to an external array
		void setIndices(unsigned int numIndices, const unsigned short* indices);
		/// Sets the indices data to the data used by another sprite
		void setIndices(const MeshSprite& meshSprite);

		/// Returns the internal indices data, cleared and set to the required size
		unsigned short* emplaceIndices(unsigned int numIndices);

		inline static ObjectType sType() {
			return ObjectType::MeshSprite;
		}

	protected:
		/// Protected copy constructor used to clone objects
		MeshSprite(const MeshSprite& other);

	private:
		/// The array of vertex positions, interleaved with texture coordinates when a texture is attached
		SmallVector<float, 0> vertices_;
		/// Pointer to vertex data, either from a shared array or unique to this sprite
		const float* vertexDataPointer_;
		/// The number of bytes used by each vertex
		unsigned int bytesPerVertex_;
		/// The number of vertices, either shared or not, that composes the mesh
		unsigned int numVertices_;

		/// The array of indices used to draw the sprite mesh
		SmallVector<unsigned short, 0> indices_;
		/// Pointer to index data, either from a shared array or unique to this sprite
		const unsigned short* indexDataPointer_;
		/// The number of indices, either shared or not, that composes the mesh
		unsigned int numIndices_;

		/// Deleted assignment operator
		MeshSprite& operator=(const MeshSprite&) = delete;

		/// Initializer method for constructors and the copy constructor
		void init();

		void shaderHasChanged() override;
		void textureHasChanged(Texture* newTexture) override;
	};

}
