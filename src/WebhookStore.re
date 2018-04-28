open Js.Promise;

open PouchdbImpl;

let connection = DbHelper.init("webhooks");

let db = connection.local;

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

let getAll = () : Most.stream(Webhook.t) =>
  db
  |> find(
       Pouchdb.QueryBuilder.query(~selector={
                                    "_id": {
                                      "$gt": Js.null,
                                    },
                                  }, ()),
     )
  |> Most.fromPromise
  |> Most.awaitPromises
  |> Most.flatMap(response => {
       Js.log(
         "WebhookStore:: got Webhooks: "
         ++ string_of_int(response##docs |> Array.length),
       );
       response##docs |> Array.to_list |> Most.fromList;
     })
  |> Most.map(doc => doc |> Webhook.fromJs);

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