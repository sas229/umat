
#ifndef TYPES_H
#define TYPES_H

#include <Eigen/Core>
#include <iostream>
 
/** 
 * @brief Eigen vector with six indices for storing the Voigt tensor. 
 */
typedef Eigen::Vector<double, 6> Voigt;

/** 
 * @brief Eigen square matrix with three indices for storing the Cauchy tensor. 
 */
typedef Eigen::Matrix<double, 3, 3> Cauchy;

/** 
 * @brief Eigen square matrix with six indices for storing the constitutive matrix. 
 */
typedef Eigen::Matrix<double, 6, 6> Constitutive;

/** 
 * @brief Eigen square matrix with six indices for storing the Jacobian matrix. 
 */
typedef Eigen::Matrix<double, 6, 6> Jacobian;

/**
 * @brief Vector of parameters.
 */
typedef Eigen::VectorXd Parameters;

/**
 * @brief Vector of state variables.
 */
typedef Eigen::VectorXd State;

#endif