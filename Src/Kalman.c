/*********************************************************************************************
 ___                  _      ___   _____  ___
 |  _`\               ( )    (  _`\(_   _)(  _`\
 | (_) )   _      ___ | |/') | (_(_) | |  | (_(_)
 | ,  /  /'_`\  /'___)| , <  |  _)_  | |  `\__ \
 | |\ \ ( (_) )( (___ | |\`\ | (_( ) | |  ( )_) |
 (_) (_)`\___/'`\____)(_) (_)(____/' (_)  `\____)

 Copyright (c) 2016-2017
 All rights reserved

 RockETS, Montreal
 Ecole de Technologies Superieures
 *********************************************************************************************/
#include "Kalman.h"

//maps X (state variable) to Z (sensor data)
float32_t H[3] = { BAROMETER_H_GAIN, 0, 0 };
const arm_matrix_instance_f32 H_f32 =
    { .numCols = 3, .numRows = 1, .pData = H, };

float32_t H_t[3] = { BAROMETER_H_GAIN, 0, 0 };
const arm_matrix_instance_f32 H_t_f32 = { .numCols = 1, .numRows = 3, .pData =
    H_t, };

//measurement noise covariance
float32_t R[1] = { ALTITUDE_SIGMA * ALTITUDE_SIGMA };
const arm_matrix_instance_f32 R_f32 =
    { .numCols = 1, .numRows = 1, .pData = R, };

//process noise covariance matrix
float32_t Q[9] = { VAR_MODEL_S, 0, 0, 0, VAR_MODEL_V, 0, 0, 0, VAR_MODEL_A };
const arm_matrix_instance_f32 Q_f32 =
    { .numCols = 3, .numRows = 3, .pData = Q, };

//maps previous state to next state (throughout motion equation)
float32_t A[9] = { 1, KALMAN_TIME_STEP, KALMAN_TIME_STEP * KALMAN_TIME_STEP
    * 0.5, 0, 1, KALMAN_TIME_STEP, 0, 0, 1 };

arm_matrix_instance_f32 A_f32 = { .numCols = 3, .numRows = 3, .pData = A, };

float32_t A_t[9] = { 0, 0, 0,
KALMAN_TIME_STEP, 1, 0,
KALMAN_TIME_STEP * KALMAN_TIME_STEP * 0.5, KALMAN_TIME_STEP, 1 };
arm_matrix_instance_f32 A_t_f32 = { .numCols = 3, .numRows = 3, .pData = A_t, };

//error covariance  matrix, first guess is identity
float32_t P0[9] = { VAR_MODEL_S, 0, 0, 0, VAR_MODEL_V, 0, 0, 0, VAR_MODEL_A };
arm_matrix_instance_f32 P0_f32 = { .numCols = 3, .numRows = 3, .pData = P0, };

float32_t P1[9] = { VAR_MODEL_S, 0, 0, 0, VAR_MODEL_V, 0, 0, 0, VAR_MODEL_A };
arm_matrix_instance_f32 P1_f32 = { .numCols = 3, .numRows = 3, .pData = P1, };

float32_t P_past[9] =
    { VAR_MODEL_S, 0, 0, 0, VAR_MODEL_V, 0, 0, 0, VAR_MODEL_A };
arm_matrix_instance_f32 P_past_f32 = { .numCols = 3, .numRows = 3, .pData =
    P_past, };

float32_t K[3] = { 0.72, 0.39, 0.71 };
arm_matrix_instance_f32 K_f32 = { .numCols = 1, .numRows = 3, .pData = K, };

float32_t K_last[3] = { 0.5, 0.5, 0.5 };
arm_matrix_instance_f32 K_last_f32 = { .numCols = 1, .numRows = 3, .pData =
    K_last, };

//state matrix
float32_t X0[3] = { 0.1 };
arm_matrix_instance_f32 X0_f32 = { .numCols = 1, .numRows = 3, .pData = X0, };

float32_t X1[3] = { 0.1 };
arm_matrix_instance_f32 X1_f32 = { .numCols = 1, .numRows = 3, .pData = X1, };

float32_t X_past[3] = { 0.1 };
arm_matrix_instance_f32 X_past_f32 = { .numCols = 1, .numRows = 3, .pData =
    X_past, };

float32_t Z[1] = { 0 };
arm_matrix_instance_f32 Z_f32 = { .numCols = 1, .numRows = 1, .pData = Z, };

float32_t Y[1] = { 0 };
arm_matrix_instance_f32 Y_f32 = { .numCols = 1, .numRows = 1, .pData = Y, };

float32_t I_3x3[9] = { 1, 0, 0, 0, 1, 0, 0, 0, 1 };
arm_matrix_instance_f32 I_3x3_f32 =
    { .numCols = 3, .numRows = 3, .pData = I_3x3, };

/*********************************************************************************************
 *
 *
 * ARGUMENTS :
 *    -
 *
 * RETURN :
 *    -
 *
 *********************************************************************************************/
void Init_Kalman(Kalman_t * temp_kalman) {
  temp_kalman->Filtered_Acceleration = 0;
  temp_kalman->Filtered_Altitude = 0;
  temp_kalman->Filtered_Velocity = 0;
  temp_kalman->Filter_Converge = 1;
  temp_kalman->Acceleration = 0;
  temp_kalman->Barometric_Altitude = 0;

  temp_kalman->isInitialized = 1;
}

/*********************************************************************************************
 *
 *
 * ARGUMENTS :
 *    -
 *
 * RETURN :
 *    -
 *
 *********************************************************************************************/
void Update_Kalman(Kalman_t * temp_kalman) {
  uint32_t loop_counter = 0;
/*
  float32_t temp1_2x1[2] = { 0 };
  arm_matrix_instance_f32 temp1_2x1_f32 = { .numCols = 1, .numRows = 2, .pData =
      temp1_2x1, };
*/
  float32_t temp1_3x1[2] = { 0 };
  arm_matrix_instance_f32 temp1_3x1_f32 = { .numCols = 1, .numRows = 3, .pData =
      temp1_3x1, };

  float32_t temp1_3x3[9] = { 0 };
  arm_matrix_instance_f32 temp1_3x3_f32 = { .numCols = 3, .numRows = 3, .pData =
      temp1_3x3, };

  float32_t temp2_3x3[9] = { 0 };
  arm_matrix_instance_f32 temp2_3x3_f32 = { .numCols = 3, .numRows = 3, .pData =
      temp2_3x3, };
/*
  float32_t temp1_2x3[6] = { 0 };
  arm_matrix_instance_f32 temp1_2x3_f32 = { .numCols = 3, .numRows = 2, .pData =
      temp1_2x3, };

  float32_t temp1_2x2[4] = { 0 };
  arm_matrix_instance_f32 temp1_2x2_f32 = { .numCols = 2, .numRows = 2, .pData =
      temp1_2x2, };

  float32_t temp2_2x2[4] = { 0 };
  arm_matrix_instance_f32 temp2_2x2_f32 = { .numCols = 2, .numRows = 2, .pData =
      temp2_2x2, };

  float32_t temp1_3x2[6] = { 0 };
  arm_matrix_instance_f32 temp1_3x2_f32 = { .numCols = 2, .numRows = 3, .pData =
      temp1_3x2, };
*/
  float32_t temp1_1x3[3] = { 0 };
  arm_matrix_instance_f32 temp1_1x3_f32 = { .numCols = 3, .numRows = 1, .pData =
      temp1_1x3, };

  float32_t temp1_1x1[1] = { 0 };
  arm_matrix_instance_f32 temp1_1x1_f32 = { .numCols = 1, .numRows = 1, .pData =
      temp1_1x1, };

  float32_t temp2_1x1[1] = { 0 };
  arm_matrix_instance_f32 temp2_1x1_f32 = { .numCols = 1, .numRows = 1, .pData =
      temp2_1x1, };

  for (int i = X1_f32.numRows * X1_f32.numCols - 1; i >= 0; i--) {
    X_past[i] = X1[i];
  }

  for (int i = P1_f32.numRows * P1_f32.numCols - 1; i >= 0; i--) {

    P_past[i] = P1[i];
  }

  // measurement update
  // Z = H * x(k|k) + noise = real world
  Z[0] = temp_kalman->Barometric_Altitude;
  //Z[1] = temp_kalman->Acceleration[0];

  do {
    // Predicted state, a priori estimate
    // x(k|k-1) = A * x(k-1|k-1)
    arm_mat_mult_f32(&A_f32, &X_past_f32, &X0_f32);

    // Predicted covariance, a priori estimate
    // P(k|k-1) = A * P(k-1|k-1) * A' + Q
    arm_mat_mult_f32(&A_f32, &P_past_f32, &temp1_3x3_f32);
    arm_mat_mult_f32(&temp1_3x3_f32, &A_t_f32, &temp2_3x3_f32);
    arm_mat_add_f32(&temp2_3x3_f32, &Q_f32, &P0_f32);

    // Innovation (or measurement residual)
    // y = Z - H * x(k|k-1)
    arm_mat_mult_f32(&H_f32, &X0_f32, &temp1_1x1_f32);
    arm_mat_sub_f32(&Z_f32, &temp1_1x1_f32, &Y_f32);

    // Innovation (or residual) covariance
    // S = H * P(k|k-1) * H' + R
    arm_mat_mult_f32(&H_f32, &P0_f32, &temp1_1x3_f32);
    arm_mat_mult_f32(&temp1_1x3_f32, &H_t_f32, &temp1_1x1_f32);
    arm_mat_add_f32(&temp1_1x1_f32, &R_f32, &temp2_1x1_f32);

    // Optimal Kalman gain
    // K = P(k|k-1) * H' * S^-1
    arm_mat_inverse_f32(&temp2_1x1_f32, &temp1_1x1_f32);
    arm_mat_mult_f32(&P0_f32, &H_t_f32, &temp1_3x1_f32);
    arm_mat_mult_f32(&temp1_3x1_f32, &temp1_1x1_f32, &K_f32);

    // Updated (a posteriori) state estimate
    // X(k|k) = x(k|k-1) + K * y
    arm_mat_mult_f32(&K_f32, &Y_f32, &temp1_3x1_f32);
    arm_mat_add_f32(&X0_f32, &temp1_3x1_f32, &X1_f32);

    // Updated (a posteriori) estimate covariance
    // P(k|k) = (I - K * H) * P(k|k-1)
    arm_mat_mult_f32(&K_f32, &H_f32, &temp1_3x3_f32);
    arm_mat_sub_f32(&I_3x3_f32, &temp1_3x3_f32, &temp2_3x3_f32);
    arm_mat_mult_f32(&temp2_3x3_f32, &P0_f32, &P1_f32);

    temp_kalman->Filter_Converge = 1;

    for (int i = K_last_f32.numRows * K_last_f32.numCols - 1; i >= 0; i--) {
      if ((K[i] - K_last[i]) / K_last[i] < CONVERGENCE_PRECISION) {
        temp_kalman->Filter_Converge &= 1;
      } else {
        temp_kalman->Filter_Converge = 0;
      }
      K_last[i] = K[i];
    }

    loop_counter++;

  } while (temp_kalman->Filter_Converge == 0
      && loop_counter < CONVERGENCE_TIMEOUT);

  //update handle values
  temp_kalman->Filtered_Altitude = X1[0];
  temp_kalman->Filtered_Velocity = X1[1];
  temp_kalman->Filtered_Acceleration = X1[2];
}

