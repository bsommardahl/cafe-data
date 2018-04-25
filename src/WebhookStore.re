open Js.Promise;

open PouchdbImpl;

let db = DbHelper.init("webhooks", event => Js.log(event));

let add = (newWebhook: Webhook.New.t) =>
  db
  |> post(newWebhook |> Webhook.New.toJs)
  |> then_(revResponse =>
       db
       |> get(revResponse##id)
       |> then_(js => {
            let o = js |> Webhook.fromJs;
            Js.log(
              "WebhookStore:: added new Webhook for " ++ newWebhook.name,
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
       let mapped: array(Webhook.t) =
         docs |> Array.map(d => Webhook.fromJs(d));
       Js.log(
         "WebhookStore:: got Webhooks: "
         ++ string_of_int(docs |> Array.length),
       );
       resolve(mapped);
     })
  |> then_(expenses => resolve(Array.to_list(expenses)));

let remove = (discoundId: string) : t(unit) =>
  db
  |> PouchdbImpl.get(discoundId)
  |> then_(item =>
       db
       |> remove(item)
       |> then_((_) => {
            Js.log("WebhookStore:: removed Webhook with id: " ++ discoundId);
            resolve();
          })
     );