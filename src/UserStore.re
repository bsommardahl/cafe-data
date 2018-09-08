open Js.Promise;

open PouchdbImpl;

let connection = DbHelper.init("users");

type item = User.t;
type newItem = User.New.t;

let id = (item: item) => item.id;

let db = connection.local;

let add = (newUser: User.New.t) =>
  db
  |> post(newUser |> User.New.toJs)
  |> then_(revResponse =>
       db
       |> get(revResponse##id)
       |> then_(js => {
            let o = js |> User.fromJs;
            Js.log("UserStore:: added new User for " ++ newUser.name);
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
       let mapped: array(User.t) = docs |> Array.map(d => User.fromJs(d));
       Js.log(
         "UserStore:: got Users: " ++ string_of_int(docs |> Array.length),
       );
       resolve(mapped);
     })
  |> then_(expenses => resolve(Array.to_list(expenses)));

let update = (expense: User.t): Js.Promise.t(User.t) =>
  db
  |> get(expense.id)
  |> then_(js => {
       let modified = expense |> User.toJsWithRev(js##_id, js##_rev);
       db
       |> put(modified)
       |> then_(_rev => {
            Js.log("UserStore:: updated User for " ++ js##name);
            resolve(expense);
          });
     });

let remove = (~id: string): t(unit) =>
  db
  |> PouchdbImpl.get(id)
  |> then_(item =>
       db
       |> remove(item)
       |> then_(_ => {
            Js.log("UserStore:: removed User with id: " ++ id);
            resolve();
          })
     );