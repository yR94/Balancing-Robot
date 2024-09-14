function [Xdot] = RobotDynamic(X,U,params)


m1 = params.m1;
m2 = params.m2;
g = params.g;
I1 = params.I1;
I2 = params.I2;
C = params.C;

Xdot = zeros(4,1);
Xdot(1)= X(3);
Xdot(2)= X(4);
Xdot(3)= U/I1+C*X(4);
Xdot(4)= m1*g*cos(X(1)+X(2))-(1/I2-1/I1)*U-C*X(4);

end

