package episodes;

import java.util.Arrays;

public class Episode10 {
	public static void main(String[] args) {
		int i1 = 5;
		int i2 = 15;
		int i3 = 19;
		
		// declarations
		int[] arr1;
		arr1 = new int[5];
		
		int[] arr2 = new int[5];
		
		int[] arr3 = { 5, 15, 19, 25, -6 };
		
		// accessing and modifying elements
		for (int i = 0; i < arr3.length; i++) {
			arr3[i] *= 2;
			
			System.out.println(arr3[i]);
		}
		
		// array to string
		System.out.println(Arrays.toString(arr3));
		
		// sort array
		Arrays.sort(arr3);
		System.out.println(Arrays.toString(arr3));
		
		// comparison
		for (int i = 0; i < arr2.length; i++) {
			arr2[i] = arr3[i];
		}
		System.out.println(arr2 == arr3); // false because different references
		System.out.println(Arrays.equals(arr2, arr3)); // true because tests values
	}
}
