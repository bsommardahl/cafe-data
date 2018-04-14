open Js.Promise;

open PouchdbImpl;

let expenseDb =
  PouchdbImpl.connect("expenses", Config.Database.livePouchDbConfig);

let add = (newExpense: Expense.New.t) =>
  expenseDb
  |> post(newExpense |> Expense.New.mapToJs)
  |> then_(revResponse =>
       expenseDb
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
  expenseDb
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
          "$lt": endDate |> Date.addDays(1),
        },
      },
      (),
    ),
  );

let update = (expense: Expense.t) : Js.Promise.t(Expense.t) =>
  expenseDb
  |> get(expense.id)
  |> then_(js => {
       let modified = expense |> Expense.toJsWithRev(js##_rev);
       expenseDb
       |> put(modified)
       |> then_(_rev => {
            Js.log("ExpenseStore:: updated Expense for " ++ js##name);
            resolve(Expense.fromJs(js));
          });
     });

let remove = (discoundId: string) : t(unit) =>
  expenseDb
  |> PouchdbImpl.get(discoundId)
  |> then_(item =>
       expenseDb
       |> remove(item)
       |> then_((_) => {
            Js.log("ExpenseStore:: removed Expense with id: " ++ discoundId);
            resolve();
          })
     );