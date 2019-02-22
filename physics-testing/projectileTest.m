
clear;
clc;

dt=0.1;
t=0:dt:6.5;

p=1;
A=0.038;
a=0.11;
Cs=0.5;
Cd=0.2;
w=0;
Vb=30;
Cl=(Cs*w*a)/Vb;
wx=0;
wy=1;
wz=0;
W = [wx; wy; wz];

offset=pi/2;
theta=offset-(0^pi)/180;
phi=offset-(10*pi)/180;
m=0.45;
g=9.82;

Vx(1)=Vb*sin(phi)*sin(theta);
Vy(1)=Vb*cos(phi);
Vz(1)=Vb*sin(phi)*cos(theta);

K=(p*Cd*A)/2;
Fd=K*Vb^2;

%K1=(Cs*p*A*a*w)/2;
%Fl=K1*Vb;

V = [Vx; Vy; Vz];
x(1)=0;
y(1)=0;
z(1)=0;

for i=2:100
    
    Kl=(Cl*p*A*(V(1)^2+V(2)^2+V(3)^2))/(2*norm(cross(W,V)));
    
    Ax=(-K*abs(Vx(i-1)+Vy(i-1)+Vz(i-1))*Vx(i-1)-Kl*(W(2)*Vz(i-1)-W(3)*Vy(i-1)))/m;
    Ay=(-m*g-K*abs(Vx(i-1)+Vy(i-1)+Vz(i-1))*Vy(i-1)-Kl*(W(3)*Vx(i-1)-W(1)*Vz(i-1)))/m;
    Az=(-K*abs(Vx(i-1)+Vy(i-1)+Vz(i-1))*Vz(i-1)-Kl*(W(1)*Vy(i-1)-W(2)*Vx(i-1)))/m;
    
    Vx(i)=Vx(i-1)+Ax*dt;
    Vy(i)=Vy(i-1)+Ay*dt;
    Vz(i)=Vz(i-1)+Az*dt;
    
    x(i)=x(i-1)+Vx(i-1)*dt;
    y(i)=y(i-1)+Vy(i-1)*dt;
    z(i)=z(i-1)+Vz(i-1)*dt;
    
    if(y(i) < 0) 
        break; 
    end
    
    V=[Vx(i); Vy(i); Vz(i)];
end


plot3(x,y,z);
%plot(zz);
xlabel('X-axel');
ylabel('Y-axel');
zlabel('Z-axel');
%ylim([0 100]);
%xlim([0 50]);
%zlim([0 50]);
grid on;