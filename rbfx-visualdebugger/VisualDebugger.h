#ifndef VISUAL_DEBUGGER_H
#define VISUAL_DEBUGGER_H

#define URHOVISUALDEBUGGER_EXPORT_API __declspec(dllexport)

#ifdef URHOVISUALDEBUGGER_STATIC
#  define URHOVISUALDEBUGGER_API
#  define URHOVISUALDEBUGGER_NO_EXPORT
#else
#  ifndef URHOVISUALDEBUGGER_API
#    ifdef URHOVISUALDEBUGGER_EXPORTS
/* We are building this library */
#      define URHOVISUALDEBUGGER_API URHOVISUALDEBUGGER_EXPORT_API
#    else
/* We are using this library */
#      define URHOVISUALDEBUGGER_API __declspec(dllimport)
#    endif
#  endif

#  ifndef URHOVISUALDEBUGGER_NO_EXPORT
#    define URHOVISUALDEBUGGER_NO_EXPORT 
#  endif
#endif




#include "limits.h"

#include "Urho3D/Core/Object.h"
#include "Urho3D/Core/Timer.h"
#include "Urho3D/Math/Sphere.h"
#include "Urho3D/Math/Polyhedron.h"
#include "Urho3D/Math/Frustum.h"
#include "Urho3D/Math/Ray.h"
#include "Urho3D/Scene/Node.h"



namespace Urho3D
{
	class DebugRenderer;
	class Camera;
	class Text;
	class Node;


	//Subsytem providing persistant visualization of debug geometry.
	class URHOVISUALDEBUGGER_API VisualDebugger : public Object {
		URHO3D_OBJECT(VisualDebugger, Object);

	public:
		VisualDebugger(Context* context);

		static void RegisterObject(Context* context);

		class URHOVISUALDEBUGGER_API VisualDebuggerObject : public Object {
			URHO3D_OBJECT(VisualDebuggerObject, Object);

		public:
			friend class VisualDebugger;

			VisualDebuggerObject(VisualDebugger* visDebuger, Context* context_);

			virtual void DrawDebugGeometry(DebugRenderer* debugRenderer);

			virtual void SetEnabled(bool enabled);
			void SetLifeTimeMs(unsigned int lifeTimeMs);
			void SetColor(Color color);

		protected:

			virtual void Setup();
			virtual void TearDown();

			Color color_;
			unsigned int creationTimeMS_ = 0;
			unsigned int lifetimeMS_ = 2000;
			bool depthTest_ = false;
			bool enabled_ = false;
			WeakPtr<VisualDebugger> visDebugger_;
		};

		class URHOVISUALDEBUGGER_API VisualDebuggerCircle : public VisualDebuggerObject {
			URHO3D_OBJECT(VisualDebuggerObject, VisualDebuggerObject);

		public:
			friend class VisualDebugger;
			VisualDebuggerCircle(VisualDebugger* visDebugger, Context* context_);

			virtual void DrawDebugGeometry(DebugRenderer* debugRenderer);


			Urho3D::Vector3 GetCenter() const { return center_; }
			void SetCenter(Urho3D::Vector3 val) { center_ = val; }
			Urho3D::Vector3 GetNormal() const { return normal_; }
			void SetNormal(Urho3D::Vector3 val) { normal_ = val; }
			float GetRadius() const { return radius_; }
			void SetRadius(float val) { radius_ = val; }
			int GetSteps() const { return steps_; }
			void SetSteps(int val) { steps_ = val; }
		protected:
			Vector3 center_;
			Vector3 normal_;
			float radius_;
			int steps_ = 64;
		};

		class URHOVISUALDEBUGGER_API VisualDebuggerLine : public VisualDebuggerObject {
			URHO3D_OBJECT(VisualDebuggerObject, VisualDebuggerObject);

		public:
			friend class VisualDebugger;
			VisualDebuggerLine(VisualDebugger* visDebugger, Context* context_);

			virtual void DrawDebugGeometry(DebugRenderer* debugRenderer);

			Urho3D::Vector3 GetStart() const { return start_; }
			void SetStart(Urho3D::Vector3 val) { start_ = val; }
			Urho3D::Vector3 GetEnd() const { return end_; }
			void SetEnd(Urho3D::Vector3 val) { end_ = val; }
		protected:
			Vector3 start_;
			Vector3 end_;
		};

		class URHOVISUALDEBUGGER_API VisualDebuggerRay : public VisualDebuggerObject {
			URHO3D_OBJECT(VisualDebuggerRay, VisualDebuggerObject);

		public:
			friend class VisualDebugger;
			VisualDebuggerRay(VisualDebugger* visDebugger, Context* context_);

			virtual void DrawDebugGeometry(DebugRenderer* debugRenderer);

			Urho3D::Ray GetRay() const { return ray_; }
			void SetRay(Urho3D::Ray val) { ray_ = val; }
		protected:
			Ray ray_;
		};


		class URHOVISUALDEBUGGER_API VisualDebuggerBoundingBox : public VisualDebuggerObject {
			URHO3D_OBJECT(VisualDebuggerObject, VisualDebuggerObject);

		public:
			friend class VisualDebugger;
			VisualDebuggerBoundingBox(VisualDebugger* visDebugger, Context* context_);

			virtual void DrawDebugGeometry(DebugRenderer* debugRenderer);

			Urho3D::BoundingBox GetBox() const { return box_; }
			void SetBox(Urho3D::BoundingBox val) { box_ = val; }
			bool GetSolid() const { return solid_; }
			void SetSolid(bool val) { solid_ = val; }
		protected:
			BoundingBox box_;
			bool solid_ = false;
		};

		class URHOVISUALDEBUGGER_API VisualDebuggerTriangle : public VisualDebuggerObject {
			URHO3D_OBJECT(VisualDebuggerObject, VisualDebuggerObject);

		public:
			friend class VisualDebugger;
			VisualDebuggerTriangle(VisualDebugger* visDebugger, Context* context_);

			virtual void DrawDebugGeometry(DebugRenderer* debugRenderer);

			void SetPoints(Vector3 v1, Vector3 v2, Vector3 v3) { v1_ = v1; v2_ = v2; v3_ = v3; }
			Vector3 Point0() const { return v1_; }
			Vector3 Point1() const { return v2_; }
			Vector3 Point2() const { return v3_; }
		protected:
			Vector3 v1_, v2_, v3_;
		};

		class URHOVISUALDEBUGGER_API VisualDebuggerCross : public VisualDebuggerObject {
			URHO3D_OBJECT(VisualDebuggerCross, VisualDebuggerObject);

		public:
			friend class VisualDebugger;
			VisualDebuggerCross(VisualDebugger* visDebugger, Context* context_);

			virtual void DrawDebugGeometry(DebugRenderer* debugRenderer);

			Urho3D::Vector3 GetCenter() const { return center_; }
			void SetCenter(Urho3D::Vector3 val) { center_ = val; }
			float GetSize() const { return size_; }
			void SetSize(float val) { size_ = val; }
		protected:
			Vector3 center_;
			float size_;
		};

        class URHOVISUALDEBUGGER_API VisualDebuggerFrame : public VisualDebuggerObject {
            URHO3D_OBJECT(VisualDebuggerFrame, VisualDebuggerObject);

        public:
            friend class VisualDebugger;
            VisualDebuggerFrame(VisualDebugger* visDebugger, Context* context_);

            virtual void DrawDebugGeometry(DebugRenderer* debugRenderer);

            Urho3D::Matrix3x4 GetFrame() const { return frame_; }
            void SetFrame(Urho3D::Matrix3x4 frame) { frame_ = frame; }
            void SetScale(float scale) { scale_ = scale; }
            float GetScale() const { return scale_; }
        protected:
            Matrix3x4 frame_;
            float scale_ = 1.0f;

        };

		class URHOVISUALDEBUGGER_API VisualDebuggerPolygon : public VisualDebuggerObject {
			URHO3D_OBJECT(VisualDebuggerPolygon, VisualDebuggerObject);

		public:

			friend class VisualDebugger;

			VisualDebuggerPolygon(VisualDebugger* visDebugger, Context* context_);

			virtual void DrawDebugGeometry(DebugRenderer* debugRenderer);
			void SetPoints(Vector3 v1, Vector3 v2, Vector3 v3, Vector3 v4) { v1_ = v1; v2_ = v2; v3_ = v3; v4_ = v4;}
			Vector3 Point0() const { return v1_; }
			Vector3 Point1() const { return v2_; }
			Vector3 Point2() const { return v3_; }
			Vector3 Point3() const { return v4_; }
		protected:
			Vector3 v1_, v2_, v3_, v4_;
		};

		class URHOVISUALDEBUGGER_API VisualDebuggerPolyhedron : public VisualDebuggerObject {
			URHO3D_OBJECT(VisualDebuggerObject, VisualDebuggerObject);

		public:
			friend class VisualDebugger;
			VisualDebuggerPolyhedron(VisualDebugger* visDebugger, Context* context_);

			virtual void DrawDebugGeometry(DebugRenderer* debugRenderer);

			Urho3D::Polyhedron GetPolyhedron() const { return polyhedron_; }
			void SetPolyhedron(Urho3D::Polyhedron val) { polyhedron_ = val; }
		protected:
			Polyhedron polyhedron_;
		};

		class URHOVISUALDEBUGGER_API VisualDebuggerCylinder : public VisualDebuggerObject {
			URHO3D_OBJECT(VisualDebuggerObject, VisualDebuggerObject);

		public:
			friend class VisualDebugger;
			VisualDebuggerCylinder(VisualDebugger* visDebugger, Context* context_);

			virtual void DrawDebugGeometry(DebugRenderer* debugRenderer);

			Urho3D::Vector3 GetPosition() const { return mPosition; }
			void SetPosition(Urho3D::Vector3 val) { mPosition = val; }
			float GetRadius() const { return mRadius; }
			void SetRadius(float val) { mRadius = val; }
			float GetHeight() const { return mHeight; }
			void SetHeight(float val) { mHeight = val; }
		protected:
			Vector3 mPosition;
			float mRadius;
			float mHeight;
		};

		class URHOVISUALDEBUGGER_API VisualDebuggerFrustum : public VisualDebuggerObject {
			URHO3D_OBJECT(VisualDebuggerObject, VisualDebuggerObject);

		public:
			friend class VisualDebugger;
			VisualDebuggerFrustum(VisualDebugger* visDebugger, Context* context_);

			virtual void DrawDebugGeometry(DebugRenderer* debugRenderer);

			Urho3D::Frustum GetFrustum() const { return mFrustum; }
			void SetFrustum(Urho3D::Frustum val) { mFrustum = val; }
		protected:
			Frustum mFrustum;
		};

		class URHOVISUALDEBUGGER_API VisualDebuggerQuad : public VisualDebuggerObject {
			URHO3D_OBJECT(VisualDebuggerObject, VisualDebuggerObject);

		public:
			friend class VisualDebugger;
			VisualDebuggerQuad(VisualDebugger* visDebugger, Context* context_);

			virtual void DrawDebugGeometry(DebugRenderer* debugRenderer);

			Urho3D::Vector3 GetCenter() const { return mCenter; }
			void SetCenter(Urho3D::Vector3 val) { mCenter = val; }
			float GetWidth() const { return width; }
			void SetWidth(float val) { width = val; }
			float GetHeight() const { return height; }
			void SetHeight(float val) { height = val; }
		protected:
			Vector3 mCenter;
			float width;
			float height;
		};

		class URHOVISUALDEBUGGER_API VisualDebuggerSphere : public VisualDebuggerObject {
			URHO3D_OBJECT(VisualDebuggerObject, VisualDebuggerObject);

		public:
			friend class VisualDebugger;
			VisualDebuggerSphere(VisualDebugger* visDebugger, Context* context_);

			virtual void DrawDebugGeometry(DebugRenderer* debugRenderer);

			Urho3D::Sphere GetSphere() const { return mSphere; }
			void SetSphere(Urho3D::Sphere val) { mSphere = val; }
		protected:
			Sphere mSphere;
		};

		class URHOVISUALDEBUGGER_API VisualDebuggerSphereSector : public VisualDebuggerObject {
			URHO3D_OBJECT(VisualDebuggerObject, VisualDebuggerObject);

		public:
			friend class VisualDebugger;
			VisualDebuggerSphereSector(VisualDebugger* visDebugger, Context* context_);

			virtual void DrawDebugGeometry(DebugRenderer* debugRenderer);

			Urho3D::Sphere GetSphere() const { return mSphere; }
			void SetSphere(Urho3D::Sphere val) { mSphere = val; }
			Urho3D::Quaternion GetRotation() const { return mRotation; }
			void SetRotation(Urho3D::Quaternion val) { mRotation = val; }
			float GetAngle() const { return mAngle; }
			void SetAngle(float val) { mAngle = val; }
			bool GetDrawLines() const { return mDrawLines; }
			void SetDrawLines(bool val) { mDrawLines = val; }
		protected:
			Sphere mSphere;
			Quaternion mRotation;
			float mAngle;
			bool mDrawLines;
		};

		class URHOVISUALDEBUGGER_API VisualDebuggerOrb : public VisualDebuggerObject {
			URHO3D_OBJECT(VisualDebuggerObject, VisualDebuggerObject);

		public:
			friend class VisualDebugger;
			VisualDebuggerOrb(VisualDebugger* visDebugger, Context* context_);

			virtual void DrawDebugGeometry(DebugRenderer* debugRenderer);

			Urho3D::Vector3 GetCenter() const { return mCenter; }
			void SetCenter(Urho3D::Vector3 val) { mCenter = val; }
			float GetRadius() const { return mRadius; }
			void SetRadius(float val) { mRadius = val; }
			int GetSteps() const { return mSteps; }
			void SetSteps(int val) { mSteps = val; }
			int GetNumCircles() const { return mNumCircles; }
			void SetNumCircles(int val) { mNumCircles = val; }
		protected:
			Vector3 mCenter;
			float mRadius;
			int mSteps = 32;
			int mNumCircles = 10;
		};

		class URHOVISUALDEBUGGER_API VisualDebuggerNode : public VisualDebuggerObject {
			URHO3D_OBJECT(VisualDebuggerObject, VisualDebuggerObject);

		public:
			friend class VisualDebugger;
			VisualDebuggerNode(VisualDebugger* visDebugger, Context* context_);

			virtual void DrawDebugGeometry(DebugRenderer* debugRenderer);
			float GetScale() const { return mScale; }
			void SetScale(float val) { mScale = val; }
			WeakPtr<Urho3D::Node> GetNode() const { return node_; }
			void SetNode(WeakPtr<Urho3D::Node> val) { node_ = val; }
		protected:
			float mScale = 1.0f;
			WeakPtr<Node> node_;
		};

		class URHOVISUALDEBUGGER_API VisualDebuggerUILabel : public VisualDebuggerObject {
			URHO3D_OBJECT(VisualDebuggerObject, VisualDebuggerObject);

		public:
			friend class VisualDebugger;
			VisualDebuggerUILabel(VisualDebugger* visDebugger, Context* context_);
			~VisualDebuggerUILabel();
			virtual void DrawDebugGeometry(DebugRenderer* debugRenderer);

			virtual void Setup();
			virtual void TearDown();

			virtual void SetEnabled(bool enabled);

			void UpdatePosition();

			ea::string GetText() const { return mText; }
			void SetText(ea::string val) { mText = val; }
			Urho3D::Vector3 GetCenter() const { return mCenter; }
			void SetCenter(Urho3D::Vector3 val) { mCenter = val; }
			
		protected:
			ea::string mText;
			Vector3 mCenter;
			SharedPtr<Text> mUIText;
		};



		VisualDebuggerCircle* AddCircle(const Vector3& center, const Vector3& normal, float radius, const Color& color, int steps = 64, bool depthTest = true);

		VisualDebuggerLine* AddLine(const Vector3& start, const Vector3& end, const Color& color, bool depthTest = true);

		VisualDebuggerRay* AddRay(const Ray& ray, const Color& color, bool depthTest = true);

		VisualDebuggerOrb* AddOrb(const Vector3& center, const float& radius, const Color& color, int circleSteps = 32, int numCircles = 10, bool depthTest = true);

		VisualDebuggerUILabel* AddLabel(const Vector3& center, ea::string text, Color color = Color::WHITE);

		VisualDebuggerNode* AddNode(Node* node, const float& scale, bool depthTest);

		VisualDebuggerCross* AddCross(const Vector3& center, const float& size, Color color, bool depthTest);

        VisualDebuggerFrame* AddFrame(const Matrix3x4& frame, const float& size, bool depthTest);

		VisualDebuggerTriangle* AddTriangle(Vector3 v1, Vector3 v2, Vector3 v3, Color color, bool depthTest);

		VisualDebuggerBoundingBox* AddBoundingBox(BoundingBox boundingBox, Color color, bool depthTest);

		VisualDebuggerPolygon* AddPolygon(Vector3 v1, Vector3 v2, Vector3 v3, Vector3 v4, Color color, bool depthTest);

		VisualDebuggerPolyhedron* AddPolyhedron(Polyhedron polyhedron, Color color, bool depthTest);

		VisualDebuggerCylinder* AddCylinder(Vector3 position, float radius, float height, Color color, bool depthTest);

		VisualDebuggerFrustum* AddFrustum(Frustum frustum, Color color, bool depthTest);

		VisualDebuggerQuad* AddQuad(Vector3 center, float width, float height, Color color, bool depthTest);

		VisualDebuggerSphere* AddSphere(Sphere sphere, Color color, bool depthTest);

		VisualDebuggerSphereSector* AddSphereSector(Sphere sphere, Quaternion rotation, float angle, bool drawLines, Color color, bool depthTest);

		//Draws all the debug geometry with optional max time argument.
        void DrawDebugGeometry(DebugRenderer* debugRenderer, unsigned int maxTimeMs = UINT_MAX);

		//enables or disables all drawing
		void SetEnabled(bool enabled);

		//sets the default lifetime in milliseconds for all objects created in the future.
		void SetObjectLifeTimeMs(unsigned int lifeTimeMs = 2000);

		//sets which camera to use for world to screen cordinate mapping
		void SetPrimaryCamera(Camera* camera);

		//sets the max number of objects to render at a time.
        void SetMaxRenderObjects(unsigned int maxObjects = UINT_MAX);

	protected:

        void SetupAndAddObjectToList(VisualDebuggerObject* object, bool depthTest, Color color);

		ea::vector<SharedPtr<VisualDebuggerObject>> mDebuggerObjects;
		Timer mTimer;
		unsigned int mDefaultLifetimeMs = 2000;
		WeakPtr<Camera> mCamera;
        unsigned int mMaxRenderObjects = UINT_MAX;
		bool mEnabled = true;
	};


}
#endif
