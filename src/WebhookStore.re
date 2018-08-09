open Js.Promise;

open PouchdbImpl;

let connection = DbHelper.init("webhooks");

type item = Webhook.t;
type newItem = Webhook.New.t;

let id = (item: item) => item.id;

let db = () => connection.local;

let add = (newWebhook: Webhook.New.t) =>
  db()
  |> post(newWebhook |> Webhook.New.toJs)
  |> then_(revResponse =>
       db()
       |> get(revResponse##id)
       |> then_(js => {
            let o = js |> Webhook.fromJs;
            Js.log(
              "WebhookStore:: added new Webhook for " ++ newWebhook.name,
            );
            resolve(o);
          })
     );

let getAll = () : Js.Promise.t(list(Webhook.t)) =>
  db()
  |> find(
       Pouchdb.QueryBuilder.query(~selector={
                                    "_id": {
                                      "$gt": Js.null,
                                    },
                                  }, ()),
     )
  |> then_(response => Js.Promise.resolve(response##docs))
  |> then_(docs => {
       Js.log(
         "WebhookStore:: got Webhooks: "
         ++ string_of_int(docs |> Array.length),
       );
       Js.Promise.resolve(docs);
     })
  |> then_(docs =>
       Js.Promise.resolve(
         docs |> Array.to_list |> List.map(x => Webhook.fromJs(x)),
       )
     );

let remove = (discoundId: string) : t(unit) =>
  db()
  |> PouchdbImpl.get(discoundId)
  |> then_(item =>
       db()
       |> remove(item)
       |> then_(_ => {
            Js.log("WebhookStore:: removed Webhook with id: " ++ discoundId);
            resolve();
          })
     );