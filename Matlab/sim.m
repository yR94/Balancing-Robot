


params.r = 0.01;
 params.L = 0.05;
 params.d = 0.001;
 params.m1=0.001;
params.m2 = 0.5 ;
 params.g = 9.8;
  params.C = 0.001;
 params.I1 = (1/2) * params.m1 * params.r^2;
 params.I2 = (1/12) * params.m2*(params.L^2 + params.d^2);
 
 

 U = -0.0000000001;


tspan = [0 100];
y0 = [0 pi/4 0 0];
[t,X] = ode45(@(t,X) odefcn(t,X,U,params), tspan, y0);
figure
plot(t,X(:,1),t,X(:,2));grid on;
 
 function Xdot = odefcn(t,X,U,params)
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
 
 