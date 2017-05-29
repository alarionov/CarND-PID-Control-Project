#ifndef PID_H
#define PID_H

#include <uWS/uWS.h>

class PID {
public:
  /*
  * Errors
  */
  double p_error;
  double i_error;
  double d_error;

  /*
  * Coefficients
  */
  double P [3];
  double best_P [3];

  /*
  * Twiddle
  */

  double total_error;
  double best_error;
  double dp [3];

  int twiddle_i;
  int twiddle_p;

  double twiddle_threshold = 0.2;

  bool twiddle_completed = false;
  bool restart = false;

  int iteration;
  int batch_size = 600;

  /*
  * Constructor
  */
  PID();

  /*
  * Destructor.
  */
  virtual ~PID();

  /*
  * Initialize PID.
  */
  void Init();
  void TwiddleInit();
  /*
  * Update the PID error variables given cross track error.
  */
  void UpdateError(double cte);
  double SteeringValue();
  /*
  * Calculate the total PID error.
  */
  double TotalError();

  /*
  * Twiddle Step
  */
  void TwiddleStep();
  void Restart(uWS::WebSocket<uWS::SERVER> ws);
};

#endif /*  */
