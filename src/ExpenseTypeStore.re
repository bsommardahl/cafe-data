open Js.Promise;

open PouchdbImpl;

let db =
  PouchdbImpl.connect("expensetypes", Config.Database.livePouchDbConfig);

let add = (newExpenseType: ExpenseType.New.t) =>
  db
  |> post(newExpenseType |> ExpenseType.New.toJs)
  |> then_(revResponse =>
       db
       |> get(revResponse##id)
       |> then_(js => {
            let o = js |> ExpenseType.fromJs;
            Js.log(
              "ExpenseTypeStore:: added new ExpenseType for "
              ++ newExpenseType.name,
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
       let mapped: array(ExpenseType.t) =
         docs |> Array.map(d => ExpenseType.fromJs(d));
       Js.log(
         "ExpenseTypeStore:: got ExpenseTypes: "
         ++ string_of_int(docs |> Array.length),
       );
       resolve(mapped);
     })
  |> then_(expenses => resolve(Array.to_list(expenses)));

let update = (expenseType: ExpenseType.t) : Js.Promise.t(ExpenseType.t) =>
  db
  |> get(expenseType.id)
  |> then_(js => {
       let modified =
         expenseType |> ExpenseType.toJsWithRev(js##_id, js##_rev);
       db
       |> put(modified)
       |> then_(_rev => {
            Js.log("ExpenseTypeStore:: updated ExpenseType for " ++ js##name);
            resolve(ExpenseType.fromJs(js));
          });
     });

let remove = (discoundId: string) : t(unit) =>
  db
  |> PouchdbImpl.get(discoundId)
  |> then_(item =>
       db
       |> remove(item)
       |> then_((_) => {
            Js.log(
              "ExpenseTypeStore:: removed ExpenseType with id: " ++ discoundId,
            );
            resolve();
          })
     );