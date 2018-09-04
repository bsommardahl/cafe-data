open Js.Promise;

open PouchdbImpl;

let connection = DbHelper.init("products");

type item = Product.t;
type newItem = Product.NewProduct.t;

let id = (item: item) => item.id;

let db = connection.local;

let add = (newProduct: Product.NewProduct.t) =>
  db
  |> post(newProduct |> Product.NewProduct.mapToJs)
  |> then_(revResponse =>
       db
       |> get(revResponse##id)
       |> then_(js => {
            let o = js |> Product.mapFromJs;
            Js.log(
              "productStore:: added new product for " ++ newProduct.name,
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
       let mapped: array(Product.t) =
         docs |> Array.map(d => Product.mapFromJs(d));
       Js.log(
         "productStore:: got products: "
         ++ string_of_int(docs |> Array.length),
       );
       resolve(mapped);
     })
  |> then_(products => resolve(Array.to_list(products)));

let update = (prod: Product.t): t(Product.t) =>
  db
  |> get(prod.id)
  |> then_(js => {
       let modified = prod |> Product.mapToJsWithRev(js##_id, js##_rev);
       db
       |> put(modified)
       |> then_(_rev => {
            Js.log("productStore:: updated product for " ++ js##name);
            resolve(prod);
          });
     });

let get = (~id: string) =>
  db
  |> get(id)
  |> then_(js => {
       let product = Product.mapFromJs(js);
       Js.Promise.resolve(product);
     });

let remove = (~id: string): t(unit) =>
  db
  |> PouchdbImpl.get(id)
  |> then_(item =>
       db
       |> remove(item)
       |> then_(_ => {
            Js.log("ProductStore:: removed product with id: " ++ id);
            resolve();
          })
     );