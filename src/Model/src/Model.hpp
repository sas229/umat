#ifndef MODEL_H
#define MODEL_H

#include <iostream>
#include <vector> 
#include <string>
#include <plog/Log.h>
#include <Eigen/Eigen>

class Model {
    public:
        /** @brief Method to set stress tensor in Voigt notation. */
        void set_sigma(Eigen::VectorXd s);
        /** @brief Method to set Jacobian matrix. */
        void set_jacobian(Eigen::MatrixXd j);
        /** @brief Virtual method to set state variables. */
        virtual void set_state_variables(std::vector<double> s) {};
        /** @brief Virtual method to set model parameters. */
        virtual void set_parameters(std::vector<double> s) {};
        
        /** @brief Method to get stress tensor in Voigt notation. */
        Eigen::VectorXd get_sigma(void);
        /** @brief Method to get Jacobian matrix. */
        Eigen::MatrixXd get_jacobian(void);
        /** @brief Method to get state variables. */
        std::vector<double> get_state_variables(void);
        
        /** @brief Method to compute the elastic matrix. */
        Eigen::Matrix<double, 6, 6> get_elastic_matrix(void);

        /** @brief Method to compute the isotropic linear elastic matrix. */
        void compute_isotropic_linear_elastic_matrix(void);
        /** @brief Method to compute the shear modulus from bulk modulus and Poisson's ratio. */
        void compute_G(void);
        /** @brief Method to compute Young's modulus from shear modulus and Poisson's ratio. */
        void compute_E(void);
        /** @brief Method to compute the bulk modulus. */
        void compute_K(void);
        /** @brief Method to compute the stress invariants. */
        void compute_invariants(void);

        virtual ~Model() {}
        
    protected:

        /** @brief Method to set name of model. */
        void set_name(std::string name);
        /** @brief Method to set number of model parameters. */
        void set_n_parameters(int i);
        /** @brief Method to set number of state variables. */
        void set_n_state_variables(int i);

        /** @brief Method to get name of model. */
        std::string get_name(void);
        /** @brief Method to get number of model parameters. */
        int get_n_parameters(void);
        /** @brief Method to get number of state variables. */
        int get_n_state_variables(void);

        /** @brief Name of model. */
        std::string name;
        /** @brief Number of model parameters. */
        int n_parameters;
        /** @brief Number of state variables. */
        int n_state_variables;
        /** @brief Stress vector in Voigt notation \f$(\sigma_{11},\sigma_{22},\sigma_{33},\sigma_{12},\sigma_{13},\sigma_{23})\f$. */
        Eigen::VectorXd sigma_v;
        /** @brief Stress tensor in full form. */
        Eigen::Matrix<double, 3, 3> sigma;
        /** @brief Deviatoric stress tensor in full form. */
        Eigen::Matrix<double, 3, 3> s;
        /** @brief Kronecker delta tensor in full form. */
        Eigen::Array<double, 3, 3> delta {{1, 0, 0}, {0, 1, 0}, {0, 0, 1}};
        /** @brief Jacobian matrix. */
        Eigen::MatrixXd jacobian;
        /** @brief Array of state variables. */
        std::vector<double> state;
        /** @brief Bulk modulus. */
        double K = 0.0;
        /** @brief Shear modulus. */
        double G = 0.0;
        /** @brief Young's modulus. */
        double E = 0.0;
        /** @brief Poisson's ratio. */
        double nu = 0.0; 
        /** @brief First stress invariant. */
        double I_1; 
        /** @brief Second stress invariant. */
        double I_2; 
        /** @brief Third stress invariant. */
        double I_3; 
        /** @brief First deviatoric stress invariant. */
        double J_1; 
        /** @brief Second deviatoric stress invariant. */
        double J_2; 
        /** @brief Third deviatoric stress invariant. */
        double J_3; 
        /** @brief Mean stress. */
        double p; 
        /** @brief Deviatoric stress. */
        double q; 
        /** @brief Major principal stress. */
        double sigma_1; 
        /** @brief Intermediate principal stress. */
        double sigma_2; 
        /** @brief Minor principal stress. */
        double sigma_3;
        /** @brief Mises stress. */
        double mises;
        /** @brief Maximum shear stress. */
        double max_shear;  
        /** @brief Elastic matrix. */
        Eigen::Matrix<double, 6, 6> D_e = Eigen::Matrix<double, 6, 6>::Zero();
};

#endif