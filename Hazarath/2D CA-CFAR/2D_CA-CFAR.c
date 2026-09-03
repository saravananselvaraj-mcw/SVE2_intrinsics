// 2D CA-CFAR (Cell-Averaging Constant False Alarm Rate): adaptively flags a cell as a target when its value exceeds a noise threshold estimated by averaging the surrounding training cells, while excluding the guard cells closest to it.
#include<stdio.h>
#include<math.h>
#include<stdlib.h>
int main()
{
    // Input of Doppler 2D array cells, row and column-wise sizes of test and Guardian cells and offset.
    int input_x = 10,input_y = 12, Guardian_x = 2, Guardian_y = 2, Test_x = 2, Test_y = 3;
    double offset = 1.2, input[10][12] = {
    {0.21477920894998642, 0.5542752837260357, -0.0698928605142817, 0.49270772521570977, 0.7745915471662987, 0.40397069107175637, 0.4159834482266725, 0.5030152904717838, 0.5502494367567672, 0.5514778054668769, 0.542220799924258, 0.6101197486166718},
    {0.5908024698000002, 0.4607461046953283, 0.5643633942015163, 0.6549531011197849, 0.491725628590297, 0.38831411824404793, 0.5347171477907239, 0.7758433157252638, 0.308649842227188, 0.5351408865106796, 0.6547473274963175, 0.5465051370691714},
    {0.4392027753117354, 0.3254806417826386, 0.8031758834567246, 0.5095882114401862, 0.27941927200363254, 0.631058273372932, 0.45657214080005093, 0.6473794370306154, 0.3959325682402981, 0.0973001884802566, 0.002899055660089897, 0.6089976353542822},
    {0.020928815880501533, 0.8082826001951648, 0.7012047721196989, -0.006062465520964968, 0.5797593045170495, 0.2790103401571059, 0.46422664377958167, 0.6872084785819127, 0.3692362128659903, 0.7555001530959384, 0.6575975267907216, 0.420157705138798},
    {0.3970839637277771, 1.0, 0.6747442850785506, 0.5615516199857783, 0.583133178320232, 0.4124380149339856, 0.04087527857035471, 0.5593045378184145, 0.10382438905022086, 0.1511607801594206, 0.46564010131652006, 0.8422412739266505},
    {0.14331679007943274, 0.7254731583111648, 0.5068951547608074, 0.5627857759993787, 0.6838273172705014, 0.1919824584842269, 0.6964448536211438, 0.19198245848422693, 0.6838273172705012, 0.562785775999379, 0.5068951547608074, 0.7254731583111648},
    {0.39145123476161586, 0.6261305274176769, 0.567819066883081, 0.6001785320579135, 0.3856011902567108, 0.5708608620024291, 0.10345139064995093, 0.6062356634231407, 0.6594689966517595, 0.6078657018191677, 0.14848677380227374, 0.4782830201281335},
    {0.5798108362818903, 0.3424552461446489, 0.7749759517879828, 0.41760586700312124, 0.6421316511704441, 0.3695655010131712, 0.6905040665370221, 0.11198122186351957, 0.3986094539633821, 0.4523970891518089, 0.6509584965435949, 0.08454876663446892},
    {0.6387936229678775, 0.3570660112954653, 0.5871563683299293, 0.33659303292503123, 0.7260859410982176, -0.08116701712703125, 0.6697336333482686, 0.5217042726805496, 0.3622673718560428, 0.625287397769625, 0.6143013613531648, 0.7113100442005531},
    {0.7205871802391222, 0.09930345804791298, 0.3674682095222305, 0.27477610864330465, 0.5640634306881035, 0.4945170315105238, 0.6396985002518845, -0.03461632292757563, 0.40406942023147213, 0.7594189388458337, 0.3471187538700676, 0.2801204126047695},
    };
    
    //Start iterating through all the Test cells and check whether they are targets or not through 2D CA-CFAR algorithm
    for(int i=(Guardian_x+Test_x);i<(input_x)-(Guardian_x+Test_x);i++)
    {
        for(int j=(Guardian_y+Test_y);j<(input_y)-(Guardian_y+Test_y);j++)
        {
            //Implementing sliding-window accross all the cells in the calculated area. 
            //Iterating through the window and check if the value is lying outside the Guardian region or not
            double Noise_supression = 0.0;
            for(int p=i-(Guardian_x+Test_x);p<i+(Guardian_x+Test_x)+1;p++)
            {
                for(int q=j-(Guardian_y+Test_y);q<j+(Guardian_y+Test_y)+1;q++)
                {
                    //If cell is u=outside the Guardian region, consider it for noise supression calculation else do not consider.
                    if(abs(i-p)>Guardian_x || abs(j-q)>Guardian_y)
                    {
                        //Convert Decible to Linear power
                        Noise_supression += pow(10,(input[p][q]/10));
                    }
                }
            }
            //Implement Cell Averaging for refernece cells.
            double Threshold = (Noise_supression)/((2*(Guardian_x+Test_x+1)*2*(Guardian_y+Test_y+1))-(Guardian_x*Guardian_y)-1);
            //Convert linear power to Decible before addding the offset.
            Threshold = (10 * log10(Threshold));
            Threshold += offset;
            //If considered CUT is greater than threshold,CUT is considered to be a target.
            if(input[i][j]>=Threshold)
            {
                input[i][j]=1.0000;
            }
        }
    }
    //Targets are the one's having value of 1 as it's target cells and all the remaining one's are 0's.
    printf("The targets are:\n");
    for(int i=0;i<10;i++)
    {
        for(int j=0;j<12;j++)
        {
            if((int)input[i][j]==0||(int)input[i][j]==1)
            {
                printf("%d ",(int)input[i][j]);
            }
            else
            {
                printf("0 ");
            }
        }
        printf("\n");
    }
    return 0;
}

