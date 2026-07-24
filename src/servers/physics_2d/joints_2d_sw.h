/**************************************************************************/
/*  joints_2d_sw.h                                                        */
/**************************************************************************/
/*                         This file is part of:                          */
/*                             GNUCHAN ENGINE                             */
/*                    https://github.com/gnuchanos/GnuChan_Engine         */
/**************************************************************************/
/* Copyright (C) 2025 GnuChan Engine contributors (see AUTHORS.md).      */
/*                                                                        */
/* This program is free software: you can redistribute it and/or modify   */
/* it under the terms of the GNU General Public License as published by   */
/* the Free Software Foundation, either version 3 of the License, or      */
/* (at your option) any later version.                                    */
/*                                                                        */
/* This program is distributed in the hope that it will be useful,        */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of         */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the           */
/* GNU General Public License for more details.                           */
/*                                                                        */
/* You should have received a copy of the GNU General Public License      */
/* along with this program. If not, see <https://www.gnu.org/licenses/>.  */
/**************************************************************************/

#ifndef JOINTS_2D_SW_H
#define JOINTS_2D_SW_H

#include "body_2d_sw.h"
#include "constraint_2d_sw.h"

class Joint2DSW : public Constraint2DSW {
	real_t max_force;
	real_t bias;
	real_t max_bias;

public:
	_FORCE_INLINE_ void set_max_force(real_t p_force) { max_force = p_force; }
	_FORCE_INLINE_ real_t get_max_force() const { return max_force; }

	_FORCE_INLINE_ void set_bias(real_t p_bias) { bias = p_bias; }
	_FORCE_INLINE_ real_t get_bias() const { return bias; }

	_FORCE_INLINE_ void set_max_bias(real_t p_bias) { max_bias = p_bias; }
	_FORCE_INLINE_ real_t get_max_bias() const { return max_bias; }

	virtual Physics2DServer::JointType get_type() const = 0;
	Joint2DSW(Body2DSW **p_body_ptr = nullptr, int p_body_count = 0) :
			Constraint2DSW(p_body_ptr, p_body_count) {
		bias = 0;
		max_force = max_bias = 3.40282e+38;
	};
};

class PinJoint2DSW : public Joint2DSW {
	union {
		struct {
			Body2DSW *A;
			Body2DSW *B;
		};

		Body2DSW *_arr[2];
	};

	Transform2D M;
	Vector2 rA, rB;
	Vector2 anchor_A;
	Vector2 anchor_B;
	Vector2 bias;
	Vector2 P;
	real_t softness;

public:
	virtual Physics2DServer::JointType get_type() const { return Physics2DServer::JOINT_PIN; }

	virtual bool setup(real_t p_step);
	virtual void solve(real_t p_step);

	void set_param(Physics2DServer::PinJointParam p_param, real_t p_value);
	real_t get_param(Physics2DServer::PinJointParam p_param) const;

	PinJoint2DSW(const Vector2 &p_pos, Body2DSW *p_body_a, Body2DSW *p_body_b = nullptr);
	~PinJoint2DSW();
};

class GrooveJoint2DSW : public Joint2DSW {
	union {
		struct {
			Body2DSW *A;
			Body2DSW *B;
		};

		Body2DSW *_arr[2];
	};

	Vector2 A_groove_1;
	Vector2 A_groove_2;
	Vector2 A_groove_normal;
	Vector2 B_anchor;
	Vector2 jn_acc;
	Vector2 gbias;
	real_t jn_max;
	real_t clamp;
	Vector2 xf_normal;
	Vector2 rA, rB;
	Vector2 k1, k2;

	bool correct;

public:
	virtual Physics2DServer::JointType get_type() const { return Physics2DServer::JOINT_GROOVE; }

	virtual bool setup(real_t p_step);
	virtual void solve(real_t p_step);

	GrooveJoint2DSW(const Vector2 &p_a_groove1, const Vector2 &p_a_groove2, const Vector2 &p_b_anchor, Body2DSW *p_body_a, Body2DSW *p_body_b);
	~GrooveJoint2DSW();
};

class DampedSpringJoint2DSW : public Joint2DSW {
	union {
		struct {
			Body2DSW *A;
			Body2DSW *B;
		};

		Body2DSW *_arr[2];
	};

	Vector2 anchor_A;
	Vector2 anchor_B;

	real_t rest_length;
	real_t damping;
	real_t stiffness;

	Vector2 rA, rB;
	Vector2 n;
	real_t n_mass;
	real_t target_vrn;
	real_t v_coef;

public:
	virtual Physics2DServer::JointType get_type() const { return Physics2DServer::JOINT_DAMPED_SPRING; }

	virtual bool setup(real_t p_step);
	virtual void solve(real_t p_step);

	void set_param(Physics2DServer::DampedStringParam p_param, real_t p_value);
	real_t get_param(Physics2DServer::DampedStringParam p_param) const;

	DampedSpringJoint2DSW(const Vector2 &p_anchor_a, const Vector2 &p_anchor_b, Body2DSW *p_body_a, Body2DSW *p_body_b);
	~DampedSpringJoint2DSW();
};

#endif // JOINTS_2D_SW_H
