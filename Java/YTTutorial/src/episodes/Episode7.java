package episodes;

public class Episode7 {
	public static String name = "Episode 7";
	
	public String instanceName;
	
	public static void run() {
		Episode7 e7 = new Episode7();
		e7.instanceName = "instance1";
		
		Episode7 e7_2 = new Episode7();
		e7_2.instanceName = "instance2";
		
		System.out.println(e7.instanceName);
		System.out.println(e7.name);
		System.out.println(e7_2.instanceName);
		System.out.println(e7_2.name);
	}
}
