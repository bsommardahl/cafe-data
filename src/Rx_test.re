open Jest;

open Expect;

open Js.Promise;

let buildWebhook = id : Webhook.t => {
  id,
  name: "test",
  url: "url1",
  event: OrderPaid,
  source: Order,
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
      let stream = webhooksPromise |> Rx.listToStream;
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
    testAsync("most", finish => {
      let add = (x, y) => x + y;
      let isEven = x => x mod 2 === 0 |> Js.Boolean.to_js_boolean;
      Most.(
        fromList([0, 1, 2, 3, 4, 5, 6, 7, 8])
        |> map(add(2))
        |> filter(isEven)
        |> observe(x => Js.log(x))
        |> Js.Promise.then_((_) => {
             Js.log("Stream completed");
             finish(expect(1) |> toBe(1));
             Js.Promise.resolve();
           })
      )
      |> ignore;
      ();
    });
  })
);