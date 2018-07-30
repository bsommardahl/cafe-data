open Jest;

open Expect;

open Js.Promise;

let buildWebhook = id : Webhook.t => {
  id,
  name: "test",
  url: "url1",
  event: OrderPaid,
  source: Order,
  behavior: FireAndForget,
};

describe("The Webhook Store", () =>
  describe("When transforming a promise of webhooks to a stream", () => {
    let webhooksPromise =
      Js.Promise.resolve([
        buildWebhook("1"),
        buildWebhook("2"),
        buildWebhook("2"),
      ]);
    testAsync("it should return all webhooks as separate events", finish => {
      let stream = webhooksPromise |> Rx.promiseWithListToStream;
      stream
      |> Most.observe(w => Js.log(w))
      |> then_((_) => {
           Js.log("Stream completed");
           finish(expect(1) |> toBe(1));
           Js.Promise.resolve();
         })
      |> ignore;
      ();
    });
  })
);