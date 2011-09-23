// kate: replace-tabs off; indent-width 4; indent-mode normal
// vim: ts=4:sw=4:noexpandtab
/*

Copyright (c) 2010--2011,
François Pomerleau and Stephane Magnenat, ASL, ETHZ, Switzerland
You can contact the authors at <f dot pomerleau at gmail dot com> and
<stephane at magnenat dot net>

All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the <organization> nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL ETH-ASL BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

#include "Core.h"

using namespace std;

template<typename T>
typename PointMatcher<T>::Vector PointMatcher<T>::TransformationChecker::matrixToAngles(const TransformationParameters& parameters)
{
	Vector angles;
	if(parameters.rows() == 4)
	{
		angles = Vector::Zero(3);

		angles(0) = atan2(parameters(2,0), parameters(2,1));
		angles(1) = acos(parameters(2,2));
		angles(2) = -atan2(parameters(0,2), parameters(1,2));
	}
	else
	{
		angles = Vector::Zero(1);

		angles(0) = acos(parameters(0,0));
	}

	return angles;
}

//--------------------------------------
// max iteration counter
template<typename T>
PointMatcher<T>::CounterTransformationChecker::CounterTransformationChecker(const Parameters& params):
	TransformationChecker("CounterTransformationChecker", CounterTransformationChecker::availableParameters(), params),
	maxIterationCount(Parametrizable::get<int>("maxIterationCount")) // FIXME: should this be unsigned?
{
	this->limits.setZero(1);
	this->limits(0) = maxIterationCount;

	this->valueNames.push_back("Iteration");
	this->limitNames.push_back("Max iteration");
}

template<typename T>
void PointMatcher<T>::CounterTransformationChecker::init(const TransformationParameters& parameters, bool& iterate)
{
	this->values.setZero(1);
}

template<typename T>
void PointMatcher<T>::CounterTransformationChecker::check(const TransformationParameters& parameters, bool& iterate)
{
	this->values(0)++;
	
	//std::cout << "Iter: " << this->values(0) << " / " << this->limits(0) << std::endl;
	//cerr << parameters << endl;
	
	if (this->values(0) >= this->limits(0))
		iterate = false;
}

template struct PointMatcher<float>::CounterTransformationChecker;
template struct PointMatcher<double>::CounterTransformationChecker;


//--------------------------------------
// error
template<typename T>
PointMatcher<T>::ErrorTransformationChecker::ErrorTransformationChecker(const Parameters& params):
	TransformationChecker("ErrorTransformationChecker", ErrorTransformationChecker::availableParameters(), params),
	minDeltaRotErr(Parametrizable::get<T>("minDeltaRotErr")),
	minDeltaTransErr(Parametrizable::get<T>("minDeltaTransErr")),
	tail(Parametrizable::get<unsigned>("tail"))
{
	this->limits.setZero(2);
	this->limits(0) = minDeltaRotErr;
	this->limits(1) = minDeltaTransErr;
	
	this->valueNames.push_back("Mean abs delta translation err");
	this->valueNames.push_back("Mean abs delta rotation err");
	this->limitNames.push_back("Min delta translation err");
	this->limitNames.push_back("Min delta rotation err");

}

template<typename T>
void PointMatcher<T>::ErrorTransformationChecker::init(const TransformationParameters& parameters, bool& iterate)
{
	this->values.setZero(4);
	
	rotations.clear();
	translations.clear();
	
	if (parameters.rows() == 4)
	{
		rotations.push_back(Quaternion(Eigen::Matrix<T,3,3>(parameters.topLeftCorner(3,3))));
	}
	else
	{
		// Handle the 2D case
		Eigen::Matrix<T,3,3> m(Matrix::Identity(3,3));
		m.topLeftCorner(2,2) = parameters.topLeftCorner(2,2);
		rotations.push_back(Quaternion(m));
	}
	
	translations.push_back(parameters.topRightCorner(parameters.rows()-1,1));
}

template<typename T>
void PointMatcher<T>::ErrorTransformationChecker::check(const TransformationParameters& parameters, bool& iterate)
{
	rotations.push_back(Quaternion(Eigen::Matrix<T,3,3>(parameters.topLeftCorner(3,3))));
	translations.push_back(parameters.topRightCorner(parameters.rows()-1,1));
	
	this->values.setZero(4);
	if(rotations.size() > tail)
	{
		for(size_t i = rotations.size()-1; i >= rotations.size()-tail; i--)
		{
			this->values(0) += anyabs(rotations[i].angularDistance(rotations[i-1]));
			this->values(1) += anyabs((translations[i] - translations[i-1]).norm());
		}

		this->values /= tail;

		if(this->values(0) < this->limits(0) && this->values(1) < this->limits(1))
			iterate = false;
	}
	
	//std::cout << "Abs Rotation: " << this->values(0) << " / " << this->limits(0) << std::endl;
	//std::cout << "Abs Translation: " << this->values(1) << " / " << this->limits(1) << std::endl;
	
	if (isnan(this->values(0)))
		throw ConvergenceError("abs rotation norm not a number");
	if (isnan(this->values(1)))
		throw ConvergenceError("abs translation norm not a number");
}

template struct PointMatcher<float>::ErrorTransformationChecker;
template struct PointMatcher<double>::ErrorTransformationChecker;

//--------------------------------------
// bound

template<typename T>
PointMatcher<T>::BoundTransformationChecker::BoundTransformationChecker(const Parameters& params):
	TransformationChecker("BoundTransformationChecker", BoundTransformationChecker::availableParameters(), params),
	maxRotationNorm(Parametrizable::get<T>("maxRotationNorm")),
	maxTranslationNorm(Parametrizable::get<T>("maxTranslationNorm"))
{
	this->limits.setZero(2);
	this->limits(0) = maxRotationNorm;
	this->limits(1) = maxTranslationNorm;

	this->limitNames.push_back("Max rotation angle");
	this->limitNames.push_back("Max translation norm");
	this->valueNames.push_back("Rotation angle");
	this->valueNames.push_back("Translation norm");
}

template<typename T>
void PointMatcher<T>::BoundTransformationChecker::init(const TransformationParameters& parameters, bool& iterate)
{
	this->values.setZero(2);
	// FIXME: handle 2D case
	assert(parameters.rows() == 4);
	initialRotation = Quaternion(Eigen::Matrix<T,3,3>(parameters.topLeftCorner(3,3)));
	initialTranslation = parameters.topRightCorner(parameters.rows()-1,1);
}

template<typename T>
void PointMatcher<T>::BoundTransformationChecker::check(const TransformationParameters& parameters, bool& iterate)
{
	const Quaternion currentRotation = Quaternion(Eigen::Matrix<T,3,3>(parameters.topLeftCorner(3,3)));
	const Vector currentTranslation = parameters.topRightCorner(parameters.rows()-1,1);
	this->values(0) = currentRotation.angularDistance(initialRotation);
	this->values(1) = (currentTranslation - initialTranslation).norm();
	if (this->values(0) > this->limits(0) || this->values(1) > this->limits(1))
	{
		ostringstream oss;
		oss << "limit out of bounds: ";
		oss << "rot: " << this->values(0) << "/" << this->limits(0) << " ";
		oss << "tr: " << this->values(1) << "/" << this->limits(1);
		throw ConvergenceError(oss.str());
	}
}

template struct PointMatcher<float>::BoundTransformationChecker;
template struct PointMatcher<double>::BoundTransformationChecker;