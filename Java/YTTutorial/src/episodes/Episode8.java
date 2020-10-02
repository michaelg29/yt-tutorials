package episodes;

class Ball {
	double radius;
}

public class Episode8 {
	public static void main(String[] args) {
		// create primitive variable
		int a = 5;
		// copy value to new location in memory
		int b = a;
		System.out.println(a + " " + b); // "5 5"
		// only change the value for a
		a = 6;
		System.out.println(a + " " + b); // "6 5"
		
		// create new reference to a Ball object
		Ball b1 = new Ball();
		b1.radius = 5.0;
		// copy the reference (now b1 and b2 point to the same object)
		Ball b2 = b1;
		System.out.println(b1.radius + " " + b2.radius); // "5.0 5.0"
		// change the value at the location that b1 points to (which is the same location that b2 points to)
		b1.radius = 5.5;
		System.out.println(b1.radius + " " + b2.radius); // "5.5 5.0"
		// tell b2 to point to a new Ball object
		b2 = new Ball();
		b2.radius = 6.0;
		System.out.println(b1.radius + " " + b2.radius); // "5.5 6.0"
	}
}
