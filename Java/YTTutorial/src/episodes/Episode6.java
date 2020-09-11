package episodes;

public class Episode6 {
	public static void run() {
		System.out.println("5 + 7 = " + add(5, 7));
		print(15, 6);
	}
	
	public static int add(int i1, int i2) {
		return i1 + i2;
	}
	
	public static void print(int i1, int i2) {
		System.out.println(i1);
		if (i1 > 10) {
			return;
		}
		System.out.println(i2);
	}
}
