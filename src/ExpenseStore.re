open Js.Promise;

open PouchdbImpl;

let connection = DbHelper.init("expenses");

type item = Expense.t;
type newItem = Expense.New.t;

let id = (item: item) => item.id;

let db = connection.local;

let add = (newExpense: Expense.New.t) =>
  db
  |> post(newExpense |> Expense.New.mapToJs)
  |> then_(revResponse =>
       db
       |> get(revResponse##id)
       |> then_(js => {
            let o = js |> Expense.fromJs;
            Js.log(
              "ExpenseStore:: added new Expense for " ++ newExpense.description,
            );
            resolve(o);
          })
     );

let query = (query: Pouchdb.QueryBuilder.queryT) =>
  db
  |> find(query)
  |> then_(response => resolve(response##docs))
  |> then_(docs => {
       let mapped: array(Expense.t) =
         docs |> Array.map(d => Expense.fromJs(d));
       Js.log(
         "ExpenseStore:: got Expenses: "
         ++ string_of_int(docs |> Array.length),
       );
       resolve(mapped);
     })
  |> then_(expenses => resolve(Array.to_list(expenses)));

let getAll = () =>
  query(
    Pouchdb.QueryBuilder.query(~selector={
                                 "_id": {
                                   "$gt": Js.null,
                                 },
                               }, ()),
  );

let getAllInRange = (startDate: Date.t, endDate: Date.t) =>
  query(
    Pouchdb.QueryBuilder.query(
      ~selector={
        "date": {
          "$gt": startDate,
          "$lt": endDate,
        },
      },
      (),
    ),
  );

let update = (expense: Expense.t) : Js.Promise.t(Expense.t) =>
  db
  |> get(expense.id)
  |> then_(js => {
       let modified = expense |> Expense.toJsWithRev(js##_rev);
       db
       |> put(modified)
       |> then_(_rev => {
            Js.log("ExpenseStore:: updated Expense for " ++ js##name);
            resolve(Expense.fromJs(js));
          });
     });

let remove = (~id: string) : t(unit) =>
  db
  |> PouchdbImpl.get(id)
  |> then_(item =>
       db
       |> remove(item)
       |> then_(_ => {
            Js.log("ExpenseStore:: removed Expense with id: " ++ id);
            resolve();
          })
     );