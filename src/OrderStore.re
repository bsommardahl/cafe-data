open Js.Promise;

open PouchdbImpl;

open OrderConversion;

open OrderData;

let db = PouchdbImpl.connect("orders", Config.Database.livePouchDbConfig);

let add = (newOrder: Order.newOrder) : Js.Promise.t(Order.order) =>
  db
  |> post(newOrder |> OrderConversion.newOrderToJs)
  |> then_(revResponse =>
       db
       |> get(revResponse##id)
       |> then_(order => {
            let o = mapOrderFromJs(order);
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
  let orderPaidDateInRange = (startDate, endDate) : t => {
    "paid": {
      "$exists": Js.Nullable.return(true),
    },
    /* "on": Js.Nullable.return({"$gt": startDate, "$lt": endDate}), */
  };
};

let query = (query: Pouchdb.QueryBuilder.queryT) =>
  db
  |> find(query)
  |> then_(response => resolve(response##docs))
  |> then_(docs => {
       let mapped: array(OrderData.Order.order) =
         docs |> Array.map(d => mapOrderFromJs(d));
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

let getClosedOrders = (startDate: Date.t, endDate: Date.t) => {
  Js.log("getting closed orders in range");
  Js.log(startDate |> Date.startOfDay);
  Js.log(endDate |> Date.endOfDay);
  query(
    Pouchdb.QueryBuilder.query(
      ~selector={
        "paid.on": {
          "$gt": startDate |> Date.startOfDay,
          "$lt": endDate |> Date.endOfDay,
        },
      },
      (),
    ),
  );
};

let update = (updateOrder: Order.updateOrder) : Js.Promise.t(Order.order) =>
  db
  |> get(updateOrder.id)
  |> Js.Promise.then_(orderJs => {
       let rev = orderJs##_rev;
       let id = orderJs##_id;
       db
       |> put(
            updateOrderToJs(
              id,
              rev,
              orderJs |> OrderConversion.mapOrderFromJs,
              updateOrder,
            ),
          )
       |> ignore;
       Js.log("orderStore:: updated order for " ++ orderJs##customerName);
       Js.Promise.resolve(mapOrderFromJs(orderJs));
     });

let get = (orderId: string) =>
  db
  |> get(orderId)
  |> then_(js => {
       let order = mapOrderFromJs(js);
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