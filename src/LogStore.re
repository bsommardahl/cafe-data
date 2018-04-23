open Js.Promise;

open PouchdbImpl;

let db = PouchdbImpl.connect("logs", Config.Database.livePouchDbConfig);

let add = (newLog: Log.New.t) =>
  db
  |> post(newLog |> Log.New.toJs)
  |> then_(revResponse =>
       db
       |> get(revResponse##id)
       |> then_(js => {
            let o = js |> Log.fromJs;
            Js.log("LogStore:: added new Log for " ++ newLog.message);
            resolve(o);
          })
     );

let query = (query: Pouchdb.QueryBuilder.queryT) =>
  db
  |> find(query)
  |> then_(response => resolve(response##docs))
  |> then_(docs => {
       let mapped: array(Log.t) = docs |> Array.map(d => Log.fromJs(d));
       Js.log(
         "LogStore:: got Logs: " ++ string_of_int(docs |> Array.length),
       );
       resolve(mapped);
     })
  |> then_(orders => resolve(Array.to_list(orders)));

let getLogs = (startDate: Date.t, endDate: Date.t) => {
  Js.log("getting closed orders in range");
  Js.log(startDate |> Date.startOfDay);
  Js.log(endDate |> Date.endOfDay);
  query(
    Pouchdb.QueryBuilder.query(
      ~selector={
        "on": {
          "$gt": startDate |> Date.startOfDay,
          "$lt": endDate |> Date.endOfDay,
        },
      },
      (),
    ),
  );
};