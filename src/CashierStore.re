open Js.Promise;

open PouchdbImpl;

let connection = DbHelper.init("cashiers");

type item = Cashier.t;
type newItem = Cashier.New.t;

let id = (item: item) => item.id;

let db = connection.local;

let add = (newCashier: Cashier.New.t) =>
  db
  |> post(newCashier |> Cashier.New.toJs)
  |> then_(revResponse =>
       db
       |> get(revResponse##id)
       |> then_(js => {
            let o = js |> Cashier.fromJs;
            Js.log(
              "CashierStore:: added new Cashier for " ++ newCashier.name,
            );
            resolve(o);
          })
     );

let getAll = () =>
  db
  |> find(
       Pouchdb.QueryBuilder.query(~selector={
                                    "_id": {
                                      "$gt": Js.null,
                                    },
                                  }, ()),
     )
  |> then_(response => resolve(response##docs))
  |> then_(docs => {
       let mapped: array(Cashier.t) =
         docs |> Array.map(d => Cashier.fromJs(d));
       Js.log(
         "CashierStore:: got Cashiers: "
         ++ string_of_int(docs |> Array.length),
       );
       resolve(mapped);
     })
  |> then_(expenses => resolve(Array.to_list(expenses)));

let update = (expense: Cashier.t) : Js.Promise.t(Cashier.t) =>
  db
  |> get(expense.id)
  |> then_(js => {
       let modified = expense |> Cashier.toJsWithRev(js##_id, js##_rev);
       db
       |> put(modified)
       |> then_(_rev => {
            Js.log("CashierStore:: updated Cashier for " ++ js##name);
            resolve(expense);
          });
     });

let remove = (~id: string) : t(unit) =>
  db
  |> PouchdbImpl.get(id)
  |> then_(item =>
       db
       |> remove(item)
       |> then_(_ => {
            Js.log("CashierStore:: removed Cashier with id: " ++ id);
            resolve();
          })
     );