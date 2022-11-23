#include "Model.hpp"

// Setters.

void Model::set_name(std::string s) {
    name = s;
}

void Model::set_jacobian(Jacobian m) {
    jacobian = m;
}

double Model::get_p_prime(void) {
    return p_prime;
}

Cauchy Model::compute_dq_dsigma_prime(Cauchy sigma_prime) {
    double q = compute_q(sigma_prime);
    if (q == 0.0) {
        return Cauchy::Zero();
    } else {
        double p_prime = compute_p_prime(sigma_prime);
        Cauchy dq_dsigma_prime;
        dq_dsigma_prime(0,0) = sigma_prime(0,0)-p_prime;
        dq_dsigma_prime(1,1) = sigma_prime(1,1)-p_prime;
        dq_dsigma_prime(2,2) = sigma_prime(2,2)-p_prime;
        dq_dsigma_prime(0,1) = 2*sigma_prime(0,1);
        dq_dsigma_prime(1,0) = 2*sigma_prime(1,0);
        dq_dsigma_prime(0,2) = 2*sigma_prime(0,2);
        dq_dsigma_prime(2,0) = 2*sigma_prime(2,0);
        dq_dsigma_prime(1,2) = 2*sigma_prime(1,2);
        dq_dsigma_prime(2,1) = 2*sigma_prime(2,1);
        dq_dsigma_prime *= 3.0/(2.0*q);
        return dq_dsigma_prime;
    }
}

Voigt Model::to_voigt(Cauchy cauchy) {
    Voigt voigt = Voigt::Zero();
    voigt(0) = cauchy(0,0);
    voigt(1) = cauchy(1,1);
    voigt(2) = cauchy(2,2);
    voigt(3) = cauchy(0,1);
    voigt(4) = cauchy(0,2);
    voigt(5) = cauchy(1,2);
    return voigt;
}

Cauchy Model::to_cauchy(Voigt voigt) {
    Cauchy cauchy = Cauchy::Zero();
    cauchy(0,0) = voigt(0);
    cauchy(1,1) = voigt(1);
    cauchy(2,2) = voigt(2);
    cauchy(0,1) = cauchy(1,0) = voigt(3);
    cauchy(0,2) = cauchy(2,0) = voigt(4);
    cauchy(1,2) = cauchy(2,1) = voigt(5);
    return cauchy;
}

void Model::set_sigma_prime(Voigt sigma_prime_tilde) {
    // Stress in Voigt notation form - change sign to use compression positive soil mechanics convention.
    this->sigma_prime_tilde = -sigma_prime_tilde;
    this->sigma_prime = to_cauchy(this->sigma_prime_tilde);

    // Total stresses given pore pressure, u.
    update_sigma();

    // Mean and deviatoric stress.
    update_p_prime();
    update_q();
}

void Model::set_strain_increment(Voigt delta_epsilon) {
    // Strain increment in Voigt notation form - change sign to use compression positive soil mechanics convention.
    this->delta_epsilon_tilde = -delta_epsilon;
    this->delta_epsilon = this->to_cauchy(delta_epsilon_tilde);
    solved = false;   
}

// Getters.

std::string Model::get_name(void) {
    return name;
}

Voigt Model::get_sigma_prime(void) {
    // Change sign back to tension positive sign convention.
    return -sigma_prime_tilde;
}

Jacobian Model::get_jacobian(void) {
    return jacobian;
}

// Computers.

Cauchy Model::compute_cartesian_stresses(Cauchy T, Cauchy S) {
    return T*S*T.transpose();
}

double Model::compute_delta_epsilon_vol(Cauchy delta_epsilon) {
    return delta_epsilon.trace();
}

void Model::compute_lode(double J_2, double J_3, double &theta_c, double &theta_s, double &theta_s_bar) {
    theta_c = 1.0/3.0*std::acos(J_3/2.0*std::pow((3.0/J_2), 3.0/2.0));
    theta_s = pi/6.0 - theta_c;
    theta_s_bar = -theta_s;
}

double Model::compute_max_shear(double sigma_1, double sigma_2, double sigma_3) {
    return std::max({std::abs(sigma_1-sigma_2), std::abs(sigma_2-sigma_3), std::abs(sigma_1-sigma_3)})/2.0;
}

double Model::compute_mises_stress(double J_2) {
    return std::sqrt(3.0*J_2);
}

double Model::compute_p(Cauchy sigma) {
    return 1.0/3.0*sigma.trace();
}

double Model::compute_p_prime(Cauchy sigma_prime) {
    return Model::compute_p(sigma_prime);
}

void Model::compute_principal_stresses(Cauchy sigma_prime, double &sigma_1, double &sigma_2, double &sigma_3, Cauchy &R, Cauchy &S) {
    // Solve eigenvalues and eigevectors..
    Eigen::EigenSolver<Eigen::MatrixXd> es(sigma_prime);
        
    // Principal stress magnitudes.
    Eigen::Vector3d principal_stresses = es.eigenvalues().real();
    S(0,0) = principal_stresses(0);
    S(1,1) = principal_stresses(1);
    S(2,2) = principal_stresses(2);

    // Sort principal stresses to allocate major, intermediate and minor appropriately.
    Eigen::Vector3d ordered_principal {principal_stresses.data()};
    std::sort(ordered_principal.data(), ordered_principal.data()+ordered_principal.size(), std::greater<int>());
    sigma_1 = ordered_principal(0);
    sigma_2 = ordered_principal(1);
    sigma_3 = ordered_principal(2);

    // Principal stress directions.
    R = es.eigenvectors().real();
}

double Model::compute_q(Cauchy sigma) {
    double q = std::sqrt(1.0/2.0*(
        (std::pow((sigma(0,0)-sigma(1,1)),2) + std::pow((sigma(1,1)-sigma(2,2)),2) + std::pow((sigma(2,2)-sigma(0,0)),2)) 
        + 6.0*(std::pow(sigma(0,1),2) + std::pow(sigma(0,2),2) + std::pow(sigma(1,2),2))));
    return q;
}

Cauchy Model::compute_s(Cauchy sigma, double p) {
    return sigma - p*eye;
}

Cauchy Model::compute_sigma(Cauchy sigma_prime, double u) {
    return sigma_prime + u*eye;
}

void Model::compute_stress_invariants(Cauchy sigma, double &I_1, double &I_2, double &I_3, double &J_1, double &J_2, double &J_3) {
    // Stress invariants.
    I_1 = sigma.trace();
    I_2 = 1.0/2.0*(std::pow(sigma.trace(),2) - (sigma.cwiseProduct(sigma)).trace());
    I_3 = sigma.determinant();

    // Mean stress.
    double p = compute_p_prime(sigma);
    
    // Deviatoric stress tensor, s.
    Cauchy s = compute_s(sigma, p);

    // Deviatoric stress invariants.
    J_1 = s.trace(); // Is always zero...
    J_2 = (std::pow(I_1,2)/3.0) - I_2;
    J_3 = s.determinant();
}

// Updaters.

void Model::update_cartesian_stresses(void) {
    this->sigma = Model::compute_cartesian_stresses(this->R, this->S);
}

void Model::update_delta_epsilon_vol(void) {
    this->delta_epsilon_vol = Model::compute_delta_epsilon_vol(this->delta_epsilon);
}

void Model::update_lode(void) {
    Model::compute_lode(this->J_2, this->J_3, this->theta_c, this->theta_s, this->theta_s_bar);
}

void Model::update_max_shear(void) {
    this->max_shear = Model::compute_max_shear(this->sigma_1, this->sigma_2, this->sigma_3);
}

void Model::update_mises_stress(void) {
    this->mises_stress = Model::compute_mises_stress(this->J_2);
}

void Model::update_p(void) {
    this->p = Model::compute_p(this->sigma);
}

void Model::update_p_prime(void) {
    this->p_prime = Model::compute_p(this->sigma_prime);
}

void Model::update_principal_stresses(void) {
    Model::compute_principal_stresses(this->sigma_prime, this->sigma_1, this->sigma_2, this->sigma_3, this->R, this->S);
}   

void Model::update_q(void) {
    this->q = Model::compute_q(this->sigma);
}

void Model::update_s(void) {
    this->s = Model::compute_s(this->sigma, this->p);
}

void Model::update_sigma(void) {
    this->sigma = Model::compute_sigma(this->sigma_prime, this->u);
}

void Model::update_stress_invariants(void) {
    Model::compute_stress_invariants(this->sigma, this->I_1, this->I_2, this->I_3, this->J_1, this->J_2, this->J_3);
}