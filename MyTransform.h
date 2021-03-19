#pragma once

#include <maths/math_utils.h>
#include <maths/matrix44.h>
#include <vector>
#include "maths/quaternion.h"

class MyTransform
{
public:
	MyTransform();

	void Set(gef::Matrix44 transform) { localTransform_ = transform; }

	void SetPosition(gef::Vector4 position) { position_ = position; }
	gef::Vector4 GetPosition() const { return position_; }
	void SetXPosition(const float x) { position_.set_x(x); }
	void SetYPosition(const float y) { position_.set_y(y); }
	void SetZPosition(const float z) { position_.set_z(z); }

	void SetRotation(gef::Vector4 rotation) { rotation_ = rotation; }
	gef::Vector4 GetRotation() const { return rotation_; }
	void SetXRotation(const float x) { rotation_.set_x(x); }
	void SetYRotation(const float y) { rotation_.set_y(y); }
	void SetZRotation(const float z) { rotation_.set_z(z); }
	gef::Quaternion GetQuaternion();


	void SetScale(gef::Vector4 scale) { scale_ = scale; }
	gef::Vector4 GetScale()	   const { return scale_; }
	void SetXScale(const float x) { scale_.set_x(x); }
	void SetYScale(const float y) { scale_.set_y(y); }
	void SetZScale(const float z) { scale_.set_z(z); }

	int childCount() const { return children_.size(); }
	MyTransform* GetParent() const { return parent_; }

	void SetParent(MyTransform* trans)
	{
		parent_ = trans;
		trans->AddChild(this);
	}

	void DeParent()
	{
		parent_->RemoveChild(this);
		parent_ = NULL;
	}

	MyTransform* GetChild(const int& n) const { return children_[n]; }
	void AddChild(MyTransform* trans) { children_.push_back(trans); }
	void RemoveChild(MyTransform* trans) 
	{ 
		for (int i = 0; i < children_.size(); i++)
		{
			if (children_[i] == trans)
				children_[i] == NULL;
		}
	}



	gef::Matrix44 GetWorldTransform() { return parent_ ? worldTransform_ : localTransform_; }
	gef::Matrix44 GetLocalTransform() { return localTransform_; }

	void InitTransform();
	void UpdateTransform();

private:

	MyTransform * parent_;

	gef::Matrix44 localTransform_;
	gef::Matrix44 worldTransform_;

	gef::Vector4 position_;
	gef::Vector4 rotation_;
	gef::Vector4 scale_;

	std::vector<MyTransform*> children_;
};

