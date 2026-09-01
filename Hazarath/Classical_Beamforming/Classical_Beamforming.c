#include <stdio.h>
#include <stdlib.h>
#include <complex.h>
#include <math.h>

void steering_vector(double Angle_rad, int Antenna_numbers,
                     double Antenna_dist, double complex *v)
{
    for (int i = 0; i < Antenna_numbers; i++)
    {
        v[i] = cexp(-2.0 * I * M_PI * Antenna_dist *
                    (double)i * sin(Angle_rad));
    }
}

double Delay_and_Sum_Beamforming(int num_samples, int Antenna_numbers,
                                 double Antenna_dist, double Min_Angle,
                                 double Max_Angle, double Angle_sum,
                                 double complex X[][10])
{
    int num_angles = (int)lround((Max_Angle - Min_Angle) / Angle_sum) + 1;

    double *angles = malloc(sizeof(double) * num_angles);
    double *responses = malloc(sizeof(double) * num_angles);

    double complex **outputs =
        malloc(sizeof(double complex *) * num_angles);

    for (int i = 0; i < num_angles; i++)
    {
        outputs[i] = malloc(sizeof(double complex) * num_samples);
    }

    /* Calculate the beam response for each scan angle. */
    for (int i = 0; i < num_angles; i++)
    {
        double Angle_deg = Min_Angle + (i * Angle_sum);
        angles[i] = Angle_deg;

        double Angle_rad = Angle_deg * (M_PI / 180.0);

        double complex w[Antenna_numbers];
        steering_vector(Angle_rad, Antenna_numbers, Antenna_dist, w);

        double complex *y = outputs[i];

        /* Apply the steering weights and sum the antenna signals. */
        for (int n = 0; n < num_samples; n++)
        {
            double complex s = 0.0;

            for (int m = 0; m < Antenna_numbers; m++)
            {
                s += X[n][m] * conj(w[m]);
            }

            y[n] = s / (double)Antenna_numbers;
        }

        /* Calculate the variance of the beamformed signal. */
        double complex mean_y = 0.0;

        for (int n = 0; n < num_samples; n++)
        {
            mean_y += y[n];
        }

        mean_y /= (double)num_samples;

        double var = 0.0;

        for (int n = 0; n < num_samples; n++)
        {
            double d = cabs(y[n] - mean_y);
            var += d * d;
        }

        var /= (double)num_samples;

        responses[i] = 10.0 * log10(var);
    }

    /* Find the angle with the maximum response. */
    double max_resp = responses[0];

    for (int i = 1; i < num_angles; i++)
    {
        if (responses[i] > max_resp)
        {
            max_resp = responses[i];
        }
    }

    for (int i = 0; i < num_angles; i++)
    {
        responses[i] -= max_resp;
    }

    int id = 0;

    for (int i = 1; i < num_angles; i++)
    {
        if (responses[i] > responses[id])
        {
            id = i;
        }
    }

    double angle_of_arrival = angles[id];

    printf("Angle of arrival: %.4f\n", angle_of_arrival);
}

int main()
{
    double complex X[5][10] = {
        {
             0.85389027 + 0.11896897 * I,
             0.69666460 - 0.71169760 * I,
            -0.29730123 - 0.90915896 * I,
            -0.87365642 + 0.07828388 * I,
            -0.58529659 + 1.11842390 * I,
             0.55546640 + 0.85601476 * I,
             1.02130987 - 0.09294449 * I,
             0.38525233 - 0.93593659 * I,
            -0.54866345 - 0.76254513 * I,
            -0.96715965 + 0.12439232 * I
        },

        {
             0.76664164 + 0.62969247 * I,
             0.85418111 - 0.41183408 * I,
            -0.05751092 - 1.04622193 * I,
            -0.66035444 - 0.58599790 * I,
            -0.98106196 + 0.49086581 * I,
            -0.05911355 + 0.69572811 * I,
             1.07131239 + 0.55763058 * I,
             0.82284905 - 0.65358731 * I,
            -0.22744990 - 0.77553594 * I,
            -0.96801588 - 0.36177181 * I
        },

        {
             0.22381305 + 1.04164448 * I,
             0.91015579 + 0.29067872 * I,
             0.67932165 - 0.74379019 * I,
            -0.46341275 - 0.93595163 * I,
            -1.25897993 - 0.11709991 * I,
            -0.61293528 + 0.72295844 * I,
             0.40359438 + 1.13886837 * I,
             1.05363762 + 0.09146186 * I,
             0.73039042 - 0.91903867 * I,
            -0.43858021 - 0.92023979 * I
        },

        {
            -0.29631709 + 1.15288117 * I,
             0.82430729 + 0.56756633 * I,
             0.84440535 - 0.24667412 * I,
             0.11560455 - 0.95365979 * I,
            -1.08070715 - 0.85376828 * I,
            -1.01858514 + 0.45139882 * I,
             0.04073744 + 1.17947305 * I,
             0.96934266 + 0.66940536 * I,
             1.00762776 - 0.51036050 * I,
            -0.00697367 - 1.04960891 * I
        },

        {
            -0.91355146 + 0.65446208 * I,
             0.23507138 + 1.22807686 * I,
             1.08702809 + 0.38506804 * I,
             0.94671503 - 0.89634679 * I,
            -0.49199050 - 1.23594469 * I,
            -1.16853178 - 0.40758970 * I,
            -0.89751429 + 0.87934276 * I,
             0.45095823 + 1.19469673 * I,
             1.29329044 + 0.23266057 * I,
             0.90291352 - 0.84050359 * I
        }
    };

    Delay_and_Sum_Beamforming(5, 10, 0.5, -90.0, 90.0, 0.05, X);

    return 0;
}
