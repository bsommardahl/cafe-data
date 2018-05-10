open Js.Promise;

open PouchdbImpl;

let connection = DbHelper.init("orders");

let db = connection.local;

let add = (newOrder: Order.newOrder) : Js.Promise.t(Order.t) =>
  db
  |> post(newOrder |> Order.newOrderToJs)
  |> then_(revResponse =>
       db
       |> get(revResponse##id)
       |> then_(order => {
            let o = Order.mapOrderFromJs(order);
            Js.log(
              "orderStore:: added new order for " ++ newOrder.customerName,
            );
            resolve(o);
          })
     );

module OrderPaidQuery = {
  type t = {
    .
    "paid": {
      .
      "$exists": Js.Nullable.t(bool),
      /* "on":
         Js.Nullable.t(
           {
             .
             "$gt": float,
             "$lt": float,
           },
         ), */
    },
  };
  let orderIsPaid = (existing: bool) : t => {
    "paid": {
      "$exists": Js.Nullable.return(existing),
    },
    /* "on": Js.Nullable.undefined, */
  };
};

let query = (query: Pouchdb.QueryBuilder.queryT) =>
  db
  |> find(query)
  |> then_(response => resolve(response##docs))
  |> then_(docs => {
       let mapped: array(Order.t) =
         docs |> Array.map(d => Order.mapOrderFromJs(d));
       Js.log(
         "OrderStore:: got Orders: " ++ string_of_int(docs |> Array.length),
       );
       resolve(mapped);
     })
  |> then_(orders => resolve(Array.to_list(orders)));

let getOpenOrders = () =>
  query(
    Pouchdb.QueryBuilder.query(
      ~selector=OrderPaidQuery.orderIsPaid(false),
      (),
    ),
  );

let getAll = () =>
  query(
    Pouchdb.QueryBuilder.query(~selector={
                                 "_id": {
                                   "$gt": Js.null,
                                 },
                               }, ()),
  );

let getClosedOrders = (startDate: Date.t, endDate: Date.t) =>
  query(
    Pouchdb.QueryBuilder.query(
      ~selector={
        "paid.on": {
          "$gt": startDate,
          "$lt": endDate,
        },
      },
      (),
    ),
  );

let update = (updateOrder: Order.updateOrder) : Js.Promise.t(Order.t) =>
  db
  |> get(updateOrder.id)
  |> Js.Promise.then_(orderJs => {
       let rev = orderJs##_rev;
       let id = orderJs##_id;
       db
       |> put(
            Order.updateOrderToJs(
              id,
              rev,
              orderJs |> Order.mapOrderFromJs,
              updateOrder,
            ),
          )
       |> ignore;
       Js.log("orderStore:: updated order for " ++ orderJs##customerName);
       Js.Promise.resolve(Order.mapOrderFromJs(orderJs));
     });

let get = (orderId: string) =>
  db
  |> get(orderId)
  |> then_(js => {
       let order = Order.mapOrderFromJs(js);
       Js.log("orderStore:: got order for " ++ js##customerName);
       Js.Promise.resolve(order);
     });

let remove = (orderId: string) : Js.Promise.t(unit) =>
  db
  |> PouchdbImpl.get(orderId)
  |> then_(order =>
       db
       |> remove(order)
       |> then_((_) => {
            Js.log("orderStore:: removed order with id: " ++ orderId);
            resolve();
          })
     );