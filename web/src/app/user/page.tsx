import { api } from "@/server/api";

export default async function Page() {
  const resp = await api.example.echo({ message: "hi" });
  if ("error" in resp) {
    return <div>Error :-(</div>;
  }

  const { message } = resp.data;

  return <>{message}</>;
}
