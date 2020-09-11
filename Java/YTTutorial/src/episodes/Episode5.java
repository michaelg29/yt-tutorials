package episodes;

public class Episode5 {
	public static void run() {
		/*
		 * boolean operators
		 */
		boolean b = true;
		b = false;
		
		double d = Math.PI;
		char c = 'a';
		int i = 10;
		
		// >, <, >=, <=, ==, !=
		b = d > 3;
		b = d < 5;
		b = (d + 5) >= (Math.PI + 5);
		b = (d - 5) <= 4;
		b = c == 'a';
		b = c != 'b';
		
		/*
		 * logical operators
		 */
		d = 15.43;
		i = 6;
		
		// &&, ||, !
		b = d > 10 && i == 6;
		b = d < 10 || i > 1;
		b = !(d < 10);
		
		/*
		 * if then
		 */
		int j;
		
		if (d < 10 && i > 6) {
			j = -1;
			System.out.println("d is less than 10");
		}
		else if (d == 10) {
			j = 0;
			System.out.println("d is equal to 10");
		}
		else {
			j = 1;
			System.out.println("d is not less than 10");
		}
		
		/*
		 * while loop
		 */
		while (d > 6) {
			System.out.println(d + " is greater than 6");
			d--;
			if (j == 1) {
				break;
			}
		}
		
		/*
		 * for loop 
		 */
		// for (initStatement; condition; postIterationStep)
		for (double d1 = 8, d2 = 4.54; 
				d1 > 4 && d2 < 7; 
				d1--, d2++) {
			if (d2 < 5) {
				continue;
			}
			System.out.println("d is greater than 4");
		}
		
		/*
		 * do while
		 */
		boolean isRunning = false;
		do {
			System.out.println("isRunning");
		} while (isRunning);
		
		/*
		 * switch
		 */
		switch (i) {
		case 1:
			System.out.println("i = 1");
			break;
		case 2:
			System.out.println("i = 2");
			break;
		default:
			System.out.println("i = ?");
			break;
		}
		
		// countdown
		i = 3;
		switch (i) {
		case 4:
			System.out.println("4");
		case 3:
			System.out.println("3");
		case 2:
			System.out.println("2");
		case 1:
			System.out.println("1");
		case 0:
			System.out.println("0");
		}
	}
}
