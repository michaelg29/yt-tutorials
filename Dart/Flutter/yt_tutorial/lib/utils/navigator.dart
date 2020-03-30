import "package:flutter/material.dart";

Map<String, Map<String, dynamic>> routes = {};

void setRoutes(List<List<dynamic>> newRoutes) {
  for (List<dynamic> item in newRoutes) {
    /*
      [
        [ route, name, callback ],
        ...
      ]
    */
    routes[item[0]] = {
      "name": item[1],
      "func": item[2],
    };
  }
}

String getName(String route) {
  return routes[route]["name"];
}

Widget buildList(BuildContext context,
    [List<String> exclusions, Map<String, Map<String, dynamic>> args]) {
  Map<String, Map<String, dynamic>> copy = routes;
  for (String key in exclusions) {
    copy.remove(key);
  }

  return ListView.builder(
    itemCount: copy.length + 1,
    itemBuilder: (context, i) {
      if (i == 0) {
        // first item
        return DrawerHeader(
          child: Text("Navigator"),
          decoration: BoxDecoration(color: Colors.redAccent),
        );
      }
      String rt = copy.keys.elementAt(i - 1);
      return ListTile(
        title: Text(getName(rt)),
        onTap: () {
          if (args != null && args.containsKey(rt)) {
            sendTo(context, rt, args[rt]);
          } else {
            sendTo(context, rt);
          }
        },
      );
    },
  );
}

void sendTo(BuildContext context, String route, [Map<String, dynamic> args]) {
  Widget result;

  if (args == null) {
    // no arguments
    result = routes[route]["func"](context);
  } else {
    // contains arguments
    result = routes[route]["func"](context, args);
  }

  Navigator.of(context).push(PageRouteBuilder(
      pageBuilder: (context, animation, secondaryAnimation) => result));
}
