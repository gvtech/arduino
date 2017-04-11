/*
  Calculates stream Linear Regression
  benefit: low memory foot print
*/

#define queueLength 10

class Fifo {
  public: 
    Fifo();
    void push(double value);
    double pop();
    int getSize();
  private:
    double _buff[queueLength];
    int _first,_last,_size;
};

class LinearRegression { 
  public:
    LinearRegression();
    void add(double X,double Y);
    void sub(double X,double Y);
    double getCoefficient();
    double getIntercept();
    double predict(double X);
  private:
    int n;
    double sum_X,sum_Y,sum_X2,sum_XY;
};


class RunningLinearRegression {
  public: 
   RunningLinearRegression(int nbPoints); 
   void add(double X,double Y);
   double getCoefficient();
   double getIntercept();
   double predict(double X);

  private:
      LinearRegression _linearRegression;
      Fifo list_X,list_Y;
      int _nbPoints;
};

