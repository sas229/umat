#ifndef MODEL_H
#define MODEL_H

#include <string>
#include <plog/Log.h>
#include <Eigen/Eigen>

class Model {
    public:
        /** @brief Method to set stress tensor. */
        void set_stress(Eigen::VectorXd stress);
        /** @brief Method to set Jacobian matrix. */
        void set_jacobian(Eigen::MatrixXd stress);

        /** @brief Method to get stress tensor. */
        Eigen::VectorXd get_stress(void);
        /** @brief Method to get Jacobian matrix. */
        Eigen::MatrixXd get_jacobian(void);
        
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
        /** @brief Stress tensor in Voigt notation \f$(\sigma_{11},\sigma_{22},\sigma_{33},\sigma_{23},\sigma_{13},\sigma_{12})\f$. */
        Eigen::VectorXd stress;
        /** @brief Jacobian matrix. */
        Eigen::MatrixXd jacobian;
};

#endif