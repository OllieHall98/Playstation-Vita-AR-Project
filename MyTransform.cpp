#include "MyTransform.h"

MyTransform::MyTransform() : 
	parent_(NULL)
{
	InitTransform();
}

void MyTransform::UpdateTransform()
{
	// Set rotation matrices
	gef::Matrix44 rX, rY, rZ;
	rX.RotationX(rotation_.x());
	rY.RotationY(rotation_.y());
	rZ.RotationZ(rotation_.z());

	// Set scale matrix
	gef::Matrix44 s;
	s.Scale(scale_);
	
	gef::Matrix44 combinedMatrix = s * rX * rY * rZ;
	combinedMatrix.SetTranslation(position_);
	
	localTransform_ = combinedMatrix;
	if (parent_) 
	{ 
		worldTransform_ = localTransform_ * parent_->GetWorldTransform(); 
	}
	else
	{
		worldTransform_ = localTransform_;
	}
}

gef::Quaternion MyTransform::GetQuaternion()
{
	double cX = cos(rotation_.x() * 0.5f);
	double sX = sin(rotation_.x() * 0.5f);
	double cY = cos(rotation_.y() * 0.5f);
	double sY = sin(rotation_.y() * 0.5f);
	double cZ = cos(rotation_.z() * 0.5f);
	double sZ = sin(rotation_.z() * 0.5f);

	gef::Quaternion q;
	q.w = cZ * cY * cX + sZ * sY * sZ;
	q.x = sZ * sY * sX - cZ * sY * sX;
	q.y = cZ * sY * cX + sZ * cY * sX;
	q.z = cZ * cY * sX - sZ * sY * cX;

	return q;
}

void MyTransform::InitTransform()
{
	localTransform_.SetIdentity();

	position_ = gef::Vector4(0.0f, 0.0f, 0.0f);
	rotation_ = gef::Vector4(0.0f, 0.0f, 0.0f);
	scale_ = gef::Vector4(1.0f, 1.0f, 1.0f);

	worldTransform_ = localTransform_;
}

