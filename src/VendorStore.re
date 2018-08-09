open Js.Promise;

open PouchdbImpl;

let connection = DbHelper.init("vendors");

type item = Vendor.t;
type newItem = Vendor.New.t;

let id = (item: item) => item.id;

let db = connection.local;

let add = (newVendor: Vendor.New.t) =>
  db
  |> post(newVendor |> Vendor.New.toJs)
  |> then_(revResponse =>
       db
       |> get(revResponse##id)
       |> then_(js => {
            let o = js |> Vendor.fromJs;
            Js.log("VendorStore:: added new Vendor for " ++ newVendor.name);
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
       let mapped: array(Vendor.t) =
         docs |> Array.map(d => Vendor.fromJs(d));
       Js.log(
         "VendorStore:: got Vendors: " ++ string_of_int(docs |> Array.length),
       );
       resolve(mapped);
     })
  |> then_(expenses => resolve(Array.to_list(expenses)));

let update = (expense: Vendor.t) : Js.Promise.t(Vendor.t) =>
  db
  |> get(expense.id)
  |> then_(js => {
       let modified = expense |> Vendor.toJsWithRev(js##_id, js##_rev);
       db
       |> put(modified)
       |> then_(_rev => {
            Js.log("VendorStore:: updated Vendor for " ++ js##name);
            resolve(Vendor.fromJs(js));
          });
     });

let remove = (~id: string) : t(unit) =>
  db
  |> PouchdbImpl.get(id)
  |> then_(item =>
       db
       |> remove(item)
       |> then_(_ => {
            Js.log("VendorStore:: removed Vendor with id: " ++ id);
            resolve();
          })
     );