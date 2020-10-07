package episodes;

class Ball {
	double radius;
}

public class Episode9 {
	public static void main(String[] args) {
		int i = 5;
		String s = "str";
		Ball b = new Ball();
		b.radius = 6.5;
		
		System.out.println(i + " " + s + " " + b.radius);
		method2(i, s, b);
		System.out.println(i + " " + s + " " + b.radius);
	}
	
	public static void method2(int a, String b, Ball c) {
		a -= 4.0;
		b += "__";
		c.radius = 2.0;
	}
}
