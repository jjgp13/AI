
import java.util.*;
import lejos.nxt.*;
import lejos.robotics.navigation.DifferentialPilot;
import lejos.robotics.objectdetection.*;

public class HelloWorld {

	static int speedC;
	static int speedB;
	static boolean collision=false;
	
	public static void collide(){
		speedC=500;
    	speedB=500;
    	
    	Motor.B.setSpeed(speedB);
    	Motor.C.setSpeed(speedC);
    	
    	Motor.B.backward();
    	Motor.C.backward();
    	
    	System.out.println("Boop");
    	
    	try {
			Thread.sleep(1000);
		} catch (InterruptedException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
	
	static class CollisionListener extends Thread{
		
		public CollisionListener(){
			
		}
		
		public void run(){
    	TouchSensor ts1 = new TouchSensor(SensorPort.S1);
    	TouchSensor ts4 = new TouchSensor(SensorPort.S4);
			while(true){
				if(ts1.isPressed() || ts4.isPressed()){
					collide();
				}
			}
		}
	}
	
    public static void main(String[] args) {
    	
    	CollisionListener listener=new CollisionListener();
    	listener.start();
    	
    	while (true){
 
    		speedC=500;
        	speedB=250;
    		
        	Motor.B.setSpeed(speedB);
        	Motor.C.setSpeed(speedC);
        	
        	Motor.B.forward();
        	Motor.C.forward();
        	
        	try {
    			Thread.sleep(8500);
    		} catch (InterruptedException e) {
    			// TODO Auto-generated catch block
    			e.printStackTrace();
    		}
        	
        	speedC=250;
        	speedB=500;
        	Motor.B.setSpeed(speedB);
        	Motor.C.setSpeed(speedC);
        	
        	Motor.B.forward();
        	Motor.C.forward();
        	
        	
        	try {
    			Thread.sleep(8500);
    		} catch (InterruptedException e) {
    			// TODO Auto-generated catch block
    			e.printStackTrace();
    		}
    	}
    	
    }
}

