open Js.Promise;

open PouchdbImpl;

let db = DbHelper.init("products", event => Js.log(event));

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

let update = (prod: Product.t) : t(Product.t) =>
  db
  |> get(prod.id)
  |> then_(js => {
       let modified = prod |> Product.mapToJsWithRev(js##_id, js##_rev);
       db
       |> put(modified)
       |> then_(_rev => {
            Js.log("productStore:: updated product for " ++ js##name);
            resolve(Product.mapFromJs(js));
          });
     });

let remove = (productId: string) : t(unit) =>
  db
  |> PouchdbImpl.get(productId)
  |> then_(item =>
       db
       |> remove(item)
       |> then_((_) => {
            Js.log("ProductStore:: removed product with id: " ++ productId);
            resolve();
          })
     );