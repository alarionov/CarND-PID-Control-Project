#include "PID.h"
#include <iostream>
#include <math.h>
#include <uWS/uWS.h>

PID::PID() {}

PID::~PID() {}

void PID::Init() {
  p_error = 0;
  i_error = 0;
  d_error = 0;

  total_error = 0;
  iteration   = 0;
}

void PID::TwiddleInit() {
  best_error  = 1e6;
  twiddle_i   = 0;
  twiddle_p   = 0;

  for (int i = 0; i < 3; i++) {
    P[i]  = 0;
    dp[i] = 1;
  }

   P[0] += dp[0];
}

void PID::UpdateError(double cte) {
  d_error  = cte - p_error;
  i_error += cte;
  p_error  = cte;

  total_error += pow(cte, 2);
  iteration++;
}

double PID::TotalError() {
  return total_error / iteration;
}

double PID::SteeringValue() {
  double p = P[0] * p_error;
  double i = P[1] * i_error;
  double d = P[2] * d_error;
  double error =  -1 * (p + i + d);

  return error;
}

void PID::TwiddleStep() {
  double error = TotalError();

  std::cout << "\nTwiddle: " << " " << twiddle_p << " " << twiddle_i;
  std::cout << " " << error << " " << best_error << "\n";
  std::cout << "P: [" << P[0] << ", " << P[1] << ", " << P[2] << "]\n";
  std::cout << "dp: [" << dp[0] << ", " << dp[1] << ", " << dp[2] << "]\n";

  if (dp[0] + dp[1] + dp[2] > twiddle_threshold) {
    if (twiddle_p == 0) {
      if (error < best_error) {
        best_error = error;
        std::copy(std::begin(P), std::end(P), std::begin(best_P));
        dp[twiddle_i] *= 1.1;
        twiddle_i = (twiddle_i + 1) % 3;
        P[twiddle_i] += dp[twiddle_i];
      } else {
        P[twiddle_i] -= 2 * dp[twiddle_i];
        twiddle_p = 1;
      }
    }
    else if (twiddle_p == 1) {
      if (error < best_error) {
        best_error = error;
        std::copy(std::begin(P), std::end(P), std::begin(best_P));
        dp[twiddle_i] *= 1.1;
      } else {
        P[twiddle_i] += dp[twiddle_i];
        dp[twiddle_i] *= 0.9;
      }
      twiddle_i = (twiddle_i + 1) % 3;
      P[twiddle_i] += dp[twiddle_i];
      twiddle_p = 0;
    }
  } else {
    std::copy(std::begin(best_P), std::end(best_P), std::begin(P));
    twiddle_completed = true;
  }
  restart = true;
}

void PID::Restart(uWS::WebSocket<uWS::SERVER> ws) {
  restart = false;
  Init();
  std::string reset_msg = "42[\"reset\",{}]";
  ws.send(reset_msg.data(), reset_msg.length(), uWS::OpCode::TEXT);
}
