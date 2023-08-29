﻿#pragma once

#include "../ContentResolver.h"

#include "Graphics/RenderCommand.h"
#include "Graphics/SceneNode.h"
#include "Graphics/Camera.h"
#include "Graphics/Viewport.h"

using namespace nCine;

namespace Jazz2::UI
{
	class UpscaleRenderPass : public SceneNode
	{
	public:
		UpscaleRenderPass()
			:
			_resizeShader(nullptr)
		{
			setVisitOrderState(SceneNode::VisitOrderState::Disabled);
		}

		virtual void Initialize(int32_t width, int32_t height, int32_t targetWidth, int32_t targetHeight);
		virtual void Register();

		bool OnDraw(RenderQueue& renderQueue) override;

		SceneNode* GetNode() const {
			return _node.get();
		}

		Vector2i GetViewSize() {
			return _view->size();
		}

	protected:
		class AntialiasingSubpass : public SceneNode
		{
			friend class UpscaleRenderPass;

		public:
			AntialiasingSubpass()
			{
				setVisitOrderState(SceneNode::VisitOrderState::Disabled);
			}

			void Register();

			bool OnDraw(RenderQueue& renderQueue) override;

		private:
			std::unique_ptr<Texture> _target;
			std::unique_ptr<Viewport> _view;
			std::unique_ptr<Camera> _camera;
			RenderCommand _renderCommand;
			Vector2f _targetSize;
		};

		std::unique_ptr<Viewport> _view;
		std::unique_ptr<Camera> _camera;
		std::unique_ptr<Texture> _target;
		Vector2f _targetSize;
		AntialiasingSubpass _antialiasing;

	private:
		std::unique_ptr<SceneNode> _node;
#if defined(ALLOW_RESCALE_SHADERS)
		Shader* _resizeShader;
#endif
		RenderCommand _renderCommand;
	};

	class UpscaleRenderPassWithClipping : public UpscaleRenderPass
	{
	public:
		UpscaleRenderPassWithClipping() : UpscaleRenderPass()
		{
		}

		void Initialize(int32_t width, int32_t height, int32_t targetWidth, int32_t targetHeight) override;
		void Register() override;

		SceneNode* GetClippedNode() const {
			return _clippedNode.get();
		}

		SceneNode* GetOverlayNode() const {
			return _overlayNode.get();
		}

		void SetClipRectangle(const Recti& scissorRect) {
			_clippedView->setScissorRect(scissorRect);
		}

	private:
		std::unique_ptr<Viewport> _clippedView;
		std::unique_ptr<Viewport> _overlayView;
		std::unique_ptr<SceneNode> _clippedNode;
		std::unique_ptr<SceneNode> _overlayNode;
	};
}